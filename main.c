#include <math.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "structs.h"

#define DegToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

#define GLUT_WINDOW_POS_Y 100
#define GLUT_WINDOW_POS_X 100

#define GLUT_WINDOW_SIZE_WIDTH 320
#define GLUT_WINDOW_SIZE_HEIGHT 240

#define NUM_OF_WALLS 100

player g_player;
key g_pressedKeys;
Line g_walls[NUM_OF_WALLS];
int g_window;
Node *g_tree;

Point calculateIntersection(Point start1, Point end1, Point start2, Point end2) {
    Point intersection;
    double x1 = start1.x;
    double y1 = start1.y;
    double x2 = end1.x;
    double y2 = end1.y;
    double x3 = start2.x;
    double y3 = start2.y;
    double x4 = end2.x;
    double y4 = end2.y;

    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator == 0) {
        intersection.x = -1;
        intersection.y = -1;
    } else {
        double numerator1 = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
        double numerator2 = (x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3);

        double t1 = numerator1 / denominator;
        double t2 = numerator2 / denominator;

        if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
            intersection.x = x1 + t1 * (x2 - x1);
            intersection.y = y1 + t1 * (y2 - y1);
        } else {
            intersection.x = -1;
            intersection.y = -1;
        }
    }

    return intersection;
}

int getSide(Line splitLine, Point point) {
    double det = (splitLine.end.x - splitLine.start.x) * (point.y - splitLine.start.y) -
                 (point.x - splitLine.start.x) * (splitLine.end.y - splitLine.start.y);

    if (det < 0) {
        return LEFT_SIDE;
    } else if (det > 0) {
        return RIGHT_SIDE;
    } else {
        return ON_SPLIT_LINE;
    }
}

Node *buildBSP(Line *lines, int numLines) {
    if (numLines == 0) {
        return NULL;
    } else {
        int randomIndex = rand() % numLines;
        Line splitLine = lines[randomIndex];

        Line leftLines[100];
        Line rightLines[100];
        int numLeftLines = 0;
        int numRightLines = 0;
        for (int i = 0; i < numLines; i++) {
            if (i == randomIndex) {
                continue;
            }
            int sideStart = getSide(splitLine, lines[i].start);
            int sideEnd = getSide(splitLine, lines[i].end);

            if ((sideStart == LEFT_SIDE && sideEnd == LEFT_SIDE) ||
                (sideStart == LEFT_SIDE && sideEnd == ON_SPLIT_LINE) ||
                (sideStart == ON_SPLIT_LINE && sideEnd == LEFT_SIDE)) {
                leftLines[numLeftLines++] = lines[i];
            } else if ((sideStart == RIGHT_SIDE && sideEnd == RIGHT_SIDE) ||
                       (sideStart == RIGHT_SIDE && sideEnd == ON_SPLIT_LINE) ||
                       (sideStart == ON_SPLIT_LINE && sideEnd == RIGHT_SIDE)) {
                rightLines[numRightLines++] = lines[i];
            } else if ((sideStart == RIGHT_SIDE && sideEnd == LEFT_SIDE) ||
                       (sideStart == LEFT_SIDE && sideEnd == RIGHT_SIDE)) {
                Point intersection = calculateIntersection(lines[i].start,
                                                           lines[i].end, splitLine.start, splitLine.end);

                Line leftPart = {lines[i].start, intersection};
                Line rightPart = {intersection, lines[i].end};
                leftPart.color[0] = lines[i].color[0];
                leftPart.color[1] = lines[i].color[1];
                leftPart.color[2] = lines[i].color[2];
                rightPart.color[0] = lines[i].color[0];
                rightPart.color[1] = lines[i].color[1];
                rightPart.color[2] = lines[i].color[2];
                leftLines[numLeftLines++] = leftPart;
                rightLines[numRightLines++] = rightPart;
            } else {
                if (rand() % 2 == 0) {
                    leftLines[numLeftLines++] = lines[i];
                } else {
                    rightLines[numRightLines++] = lines[i];
                }
            }
        }

        Node *leftSubtree = buildBSP(leftLines, numLeftLines);
        Node *rightSubtree = buildBSP(rightLines, numRightLines);

        Node *newNode = createNode(splitLine);
        newNode->left = leftSubtree;
        newNode->right = rightSubtree;
        return newNode;
    }
}

void storeTree(struct Node *root, FILE *file) {
    if (root == NULL) {
        fwrite("NULL", sizeof(char), 4, file);
        return;
    }

    fwrite(&(root->line.start.x), sizeof(double), 1, file);
    fwrite(&(root->line.start.y), sizeof(double), 1, file);
    fwrite(&(root->line.end.x), sizeof(double), 1, file);
    fwrite(&(root->line.end.y), sizeof(double), 1, file);
    fwrite(&(root->line.color[0]), sizeof(int), 1, file);
    fwrite(&(root->line.color[1]), sizeof(int), 1, file);
    fwrite(&(root->line.color[2]), sizeof(int), 1, file);

    storeTree(root->left, file);
    storeTree(root->right, file);
}

void drawLine(vec2 *start, vec2 *end, vec3 *color) {

    line *l = (line *) malloc(sizeof(line));

    float x1 = start->x;
    float y1 = start->y;
    float x2 = end->x;
    float y2 = end->y;
    float w = 320;
    float h = 240;

    x1 = 2 * x1 / w - 1;
    y1 = 2 * y1 / h - 1;

    x2 = 2 * x2 / w - 1;
    y2 = 2 * y2 / h - 1;

    start->x = x1;
    start->y = y1;
    end->x = x2;
    end->y = y2;

    l->lineColor = color;

    l->vertices = (float *) (malloc(sizeof(float) * 6));
    l->vertices[0] = start->x;
    l->vertices[1] = start->y;
    l->vertices[2] = end->x;
    l->vertices[3] = end->y;

    glGenVertexArrays(1, &l->VAO);
    glGenBuffers(1, &l->VBO);
    glBindVertexArray(l->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, l->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4,
                 &(l->vertices[0]),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint shaderProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);

    GLint colorloc = glGetUniformLocation(shaderProgram, "color");
    glUniform3fv(colorloc, 1, &(l->lineColor->x));
    glBindVertexArray(l->VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

void drawWall(int x1, int b1, int t1, int x2, int b2, int t2, int color[3]) {
    int dx = x2 - x1;
    if (dx == 0) {
        dx = 1;
    }
    int xs = x1;

    if (x1 < 0) {
        x1 = 0;
    }
    if (x2 < 0) {
        x2 = 0;
    }
    if (x1 > GLUT_WINDOW_SIZE_WIDTH) {
        x1 = GLUT_WINDOW_SIZE_WIDTH;
    }
    if (x2 > GLUT_WINDOW_SIZE_WIDTH) {
        x2 = GLUT_WINDOW_SIZE_WIDTH;
    }

    for (int x = x1; x < x2; x++) {
        int bys = (b2 - b1) * (x - xs) / dx + b1;
        int tys = (t2 - t1) * (x - xs) / dx + t1;

        if (bys < 0) {
            bys = 0;
        }
        if (tys < 0) {
            tys = 0;
        }
        if (bys > GLUT_WINDOW_SIZE_HEIGHT) {
            bys = GLUT_WINDOW_SIZE_HEIGHT;
        }
        if (tys > GLUT_WINDOW_SIZE_HEIGHT) {
            tys = GLUT_WINDOW_SIZE_HEIGHT;
        }

        drawLine(vec2_init(x, bys), vec2_init(x, tys), vec3_init(color[0], color[1], color[2]));
    }
}

void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2) {
    float da = *y1;
    float db = y2;
    float d = da - db;
    if (d == 0) {
        d = 1;
    }
    float s = da / d;
    *x1 = *x1 + s * (x2 - (*x1));
    *y1 = *y1 + s * (y2 - (*y1));
    if (*y1 == 0) {
        *y1 = 1;
    }
    *z1 = *z1 + s * (z2 - (*z1));
}

void draw3DWall(double start_x, double start_y, double end_x, double end_y, int color[3]) {
    int wx[4], wy[4], wz[4];
    int x1 = start_x - g_player.x;
    int y1 = start_y - g_player.y;

    int x2 = end_x - g_player.x;
    int y2 = end_y - g_player.y;

    wx[0] = x1 * cos(DegToRad(g_player.rotation)) - y1 * sin(DegToRad(g_player.rotation));
    wx[1] = x2 * cos(DegToRad(g_player.rotation)) - y2 * sin(DegToRad(g_player.rotation));
    wx[2] = wx[0];
    wx[3] = wx[1];

    wy[0] = y1 * cos(DegToRad(g_player.rotation)) + x1 * sin(DegToRad(g_player.rotation));
    wy[1] = y2 * cos(DegToRad(g_player.rotation)) + x2 * sin(DegToRad(g_player.rotation));
    wy[2] = wy[0];
    wy[3] = wy[1];

    wz[0] = 0 - 20;
    wz[1] = 0 - 20;
    wz[2] = 40 - 20;
    wz[3] = 40 - 20;


    if (wy[0] < 1 && wy[1] < 1) { return; }

    if (wy[0] < 1) {
        clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]);
        clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]);
    }

    if (wy[1] < 1) {
        clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]);
        clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]);
    }

    wx[0] = wx[0] * 200 / wy[0] + GLUT_WINDOW_SIZE_WIDTH / 2;
    wy[0] = wz[0] * 200 / wy[0] + GLUT_WINDOW_SIZE_HEIGHT / 2;

    wx[1] = wx[1] * 200 / wy[1] + GLUT_WINDOW_SIZE_WIDTH / 2;
    wy[1] = wz[1] * 200 / wy[1] + GLUT_WINDOW_SIZE_HEIGHT / 2;

    wx[2] = wx[2] * 200 / wy[2] + GLUT_WINDOW_SIZE_WIDTH / 2;
    wy[2] = wz[2] * 200 / wy[2] + GLUT_WINDOW_SIZE_HEIGHT / 2;

    wx[3] = wx[3] * 200 / wy[3] + GLUT_WINDOW_SIZE_WIDTH / 2;
    wy[3] = wz[3] * 200 / wy[3] + GLUT_WINDOW_SIZE_HEIGHT / 2;

    drawWall(wx[0], wy[0], wy[2], wx[1], wy[1], wy[3], color);
}

void drawTree(struct Node *node) {
    if (node == NULL) { return; }
    Point p = {g_player.x, g_player.y};
    int side = getSide(node->line, p);

    if (side == LEFT_SIDE) {
        drawTree(node->right);
        draw3DWall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
        drawTree(node->left);
    } else if (side == RIGHT_SIDE) {
        drawTree(node->left);
        draw3DWall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
        drawTree(node->right);
    } else if (node->left == NULL && node->right == NULL) {
        draw3DWall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
    }

}

double calcDist(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void move() {
    double dx = sin(DegToRad(g_player.rotation)) * 1;
    double dy = cos(DegToRad(g_player.rotation)) * 1;

    if (g_pressedKeys.w == 1) {
        for (int wall = 0; wall < sizeof(g_walls) / sizeof(g_walls[0]); wall++) {
            double lineLen = calcDist(g_walls[wall].start.x, g_walls[wall].start.y, g_walls[wall].end.x,
                                      g_walls[wall].end.y);
            double playerDist1 = calcDist(g_player.x + dx, g_player.y + dy, g_walls[wall].end.x, g_walls[wall].end.y);
            double playerDist2 = calcDist(g_walls[wall].start.x, g_walls[wall].start.y, g_player.x + dx,
                                          g_player.y + dy);
            if (playerDist1 + playerDist2 >= lineLen - 0.3f && playerDist1 + playerDist2 <= lineLen + 0.3f) { return; }
        }
        g_player.x += dx;
        g_player.y += dy;
    }
    if (g_pressedKeys.s == 1) {
        for (int wall = 0; wall < sizeof(g_walls) / sizeof(g_walls[0]); wall++) {
            double lineLen = calcDist(g_walls[wall].start.x, g_walls[wall].start.y, g_walls[wall].end.x,
                                      g_walls[wall].end.y);
            double playerDist1 = calcDist(g_player.x - dx, g_player.y - dy, g_walls[wall].end.x, g_walls[wall].end.y);
            double playerDist2 = calcDist(g_walls[wall].start.x, g_walls[wall].start.y, g_player.x - dx,
                                          g_player.y - dy);
            if (playerDist1 + playerDist2 >= lineLen - 0.3f && playerDist1 + playerDist2 <= lineLen + 0.3f) { return; }
        }
        g_player.x -= dx;
        g_player.y -= dy;
    }
    if (g_pressedKeys.a == 1) {
        g_player.rotation -= 1 * 4;
        if (g_player.rotation < 0) {
            g_player.rotation = 360;
        }
    }
    if (g_pressedKeys.d == 1) {
        g_player.rotation += 1 * 4;
        if (g_player.rotation > 359) {
            g_player.rotation = 0;
        }
    }
}

void setWallData(char *line, int lineNumber) {
    char *lineNext;
    g_walls[lineNumber].start.x = strtol(line, &lineNext, 10);
    g_walls[lineNumber].start.y = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].end.x = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].end.y = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].color[0] = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].color[1] = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].color[2] = strtol(lineNext, &lineNext, 10);

}

void fileRead(const char arg[]) {
    FILE *fp;
    char line[100];
    int lineNumber = 0;

    const char *ext = strrchr(arg, '.');
    if (!ext) {
        fp = fopen(arg, "r");
        if (fp == NULL) {
            exit(EXIT_FAILURE);
        }

        while (fgets(line, sizeof(line), fp)) {
            setWallData(line, lineNumber);
            lineNumber++;
        }
        g_tree = buildBSP(g_walls, 8);
        fclose(fp);
        fp = fopen("map.bin", "wb");
        if (fp == NULL) {
            exit(EXIT_FAILURE);
        }
        int number_of_walls = sizeof(g_walls) / sizeof(g_walls[0]);
        fwrite(&number_of_walls, sizeof(int), 1, fp);
        for (int wall = 0; wall < number_of_walls; wall++) {
            fwrite(&(g_walls[wall].start.x), sizeof(double), 1, fp);
            fwrite(&(g_walls[wall].start.y), sizeof(double), 1, fp);
            fwrite(&(g_walls[wall].end.x), sizeof(double), 1, fp);
            fwrite(&(g_walls[wall].end.y), sizeof(double), 1, fp);
            fwrite(&(g_walls[wall].color[0]), sizeof(int), 1, fp);
            fwrite(&(g_walls[wall].color[1]), sizeof(int), 1, fp);
            fwrite(&(g_walls[wall].color[2]), sizeof(int), 1, fp);
        }
        storeTree(g_tree, fp);
        fclose(fp);
    } else if (strcmp(ext + 1, "bin") == 0) {
        fp = fopen(arg, "rb");
        if (fp == NULL) {
            exit(EXIT_FAILURE);
        }
        int number_of_walls;
        fread(&number_of_walls, sizeof(int), 1, fp);
        for (int wall = 0; wall < number_of_walls; wall++) {
            fread(&(g_walls[wall].start.x), sizeof(double), 1, fp);
            fread(&(g_walls[wall].start.y), sizeof(double), 1, fp);
            fread(&(g_walls[wall].end.x), sizeof(double), 1, fp);
            fread(&(g_walls[wall].end.y), sizeof(double), 1, fp);
            fread(&(g_walls[wall].color[0]), sizeof(int), 1, fp);
            fread(&(g_walls[wall].color[1]), sizeof(int), 1, fp);
            fread(&(g_walls[wall].color[2]), sizeof(int), 1, fp);
        }
        g_tree = readTree(fp);
        fclose(fp);
    }
}

void timer() {
    glutSetWindow(g_window);
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void initialization(char arg[]) {
    timer();
    fileRead(arg);

    g_player.x = 30;
    g_player.y = 20;
    g_player.rotation = 0;

    g_pressedKeys.w = 0;
    g_pressedKeys.s = 0;
    g_pressedKeys.a = 0;
    g_pressedKeys.d = 0;
}

void render() {
    glClearColor(0, 0.2f, 0.5f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glGetError();

    move();
    drawTree(g_tree);

    printf("pos: (%f, %f) rot: %i\n", g_player.x, g_player.y, g_player.rotation);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            g_pressedKeys.w = 1;
            break;
        case 's':
            g_pressedKeys.s = 1;
            break;
        case 'a':
            g_pressedKeys.a = 1;
            break;
        case 'd':
            g_pressedKeys.d = 1;
            break;
        default:
            printf("not mapped key\n");
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            g_pressedKeys.w = 0;
            break;
        case 's':
            g_pressedKeys.s = 0;
            break;
        case 'a':
            g_pressedKeys.a = 0;
            break;
        case 'd':
            g_pressedKeys.d = 0;
            break;
        default:
            break;
    }
}

GLuint generateShaderProgram() {
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec3 color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(color, 1.0f);\n"
                                       "}\n\0";

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    glUseProgram(shaderProgram);
    return shaderProgram;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("One argument expected.");
        exit(EXIT_FAILURE);
    }

    glutInit(&argc, argv);
    glutInitContextVersion(4, 6);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowPosition(GLUT_WINDOW_POS_X, GLUT_WINDOW_POS_Y);
    glutInitWindowSize(GLUT_WINDOW_SIZE_WIDTH, GLUT_WINDOW_SIZE_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    g_window = glutCreateWindow("Goom");
    glPointSize(1);

    initialization(argv[1]);

    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        printf("GLEW error: %s\n", glewGetErrorString(glewStatus));
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, GLUT_WINDOW_SIZE_WIDTH, GLUT_WINDOW_SIZE_HEIGHT);
    generateShaderProgram();
    glutMainLoop();
    exit(EXIT_SUCCESS);
}

