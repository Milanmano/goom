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

#define NUM_OF_WALLS 8

player g_player;
key g_pressedKeys;
Line g_walls[NUM_OF_WALLS];
int g_window;
Node *g_tree;

void drawWall(int x1, int b1, int t1, int x2, int b2, int t2, int color[3]) {
    double dx = x2 - x1;
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

        draw_line(vec2_init(x, bys), vec2_init(x, tys), vec3_init(color[0], color[1], color[2]));
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

void draw_3D_Wall(double start_x, double start_y, double end_x, double end_y, int color[3]) {
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
};


void drawTree(struct Node *node) {
    if (node == NULL) { return; }
    Point p = {g_player.x, g_player.y};
    int side = getSide(node->line, p);

    if (side == LEFT_SIDE) {
        drawTree(node->right);
        draw_3D_Wall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
        drawTree(node->left);
    } else if (side == RIGHT_SIDE) {
        drawTree(node->left);
        draw_3D_Wall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
        drawTree(node->right);
    } else if (node->left == NULL && node->right == NULL) {
        draw_3D_Wall(node->line.start.x, node->line.start.y, node->line.end.x, node->line.end.y, node->line.color);
    }

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

void timer() {
    glutSetWindow(g_window);
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
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

    print_tree(g_tree);

}

void initialization(char arg[]) {
    timer();
    fileRead(arg);

    g_player.x = 80;
    g_player.y = 10;
    g_player.rotation = 0;

    g_pressedKeys.w = 0;
    g_pressedKeys.s = 0;
    g_pressedKeys.a = 0;
    g_pressedKeys.d = 0;
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

