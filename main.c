#include <math.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "structs.h"

#define DegToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

#define GLUT_WINDOW_POS_Y 100
#define GLUT_WINDOW_POS_X 100

#define GLUT_WINDOW_SIZE_WIDTH 320
#define GLUT_WINDOW_SIZE_HEIGHT 240

#define NUM_OF_WALLS 8

player g_player;
key g_pressedKeys;
wall g_walls[NUM_OF_WALLS];
int g_window[2];
tree *g_tree;

void pixel(int x, int y, int rgb[3]) {
    glColor3ub(rgb[0], rgb[1], rgb[2]);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}


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

        for (int y = bys; y < tys; y++) {
            pixel(x, y, color);
        }
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

void draw3D() {


    for (int wall = 0; wall < sizeof(g_walls) / sizeof(g_walls[0]); wall++) {
        int color[3] = {g_walls[wall].c[0], g_walls[wall].c[1], g_walls[wall].c[2]};
        int wx[4], wy[4], wz[4];
        int x1 = g_walls[wall].x1 - g_player.x;
        int y1 = g_walls[wall].y1 - g_player.y;

        int x2 = g_walls[wall].x2 - g_player.x;
        int y2 = g_walls[wall].y2 - g_player.y;

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


        if (wy[0] < 1 && wy[1] < 1) { continue; }

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
}

//TODO:
void clearScreen(int left, int right, int bottom, int top) {
    int color[3] = {0, 60, 130};
    int x, y;
    for (x = left; x < right; x++) {
        for (y = bottom; y < top; y++) {
            pixel(x, y, color);
        }
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

void move() {
    double dx = sin(DegToRad(g_player.rotation)) * 1;
    double dy = cos(DegToRad(g_player.rotation)) * 1;

    if (g_pressedKeys.w == 1) {
        g_player.x += dx;
        g_player.y += dy;
    }
    if (g_pressedKeys.s == 1) {
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
    glutSetWindow(g_window[0]);
    glutPostRedisplay();
    glutSetWindow(g_window[1]);
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearScreen(0, GLUT_WINDOW_SIZE_WIDTH, 0, GLUT_WINDOW_SIZE_HEIGHT);

    move();
    draw3D();
    printf("pos: (%f, %f) rot: %i\n", g_player.x, g_player.y, g_player.rotation);

    glutSwapBuffers();
}

void overhead() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearScreen(-GLUT_WINDOW_SIZE_WIDTH / 2, GLUT_WINDOW_SIZE_WIDTH, -GLUT_WINDOW_SIZE_HEIGHT / 2,
                GLUT_WINDOW_SIZE_HEIGHT / 2);


    for (int wall = 0; wall < sizeof(g_walls) / sizeof(g_walls[0]); wall++) {
        int wallColor[3] = {g_walls[wall].c[0], g_walls[wall].c[1], g_walls[wall].c[2]};

        int fromX, toX, fromY, toY;
        if (g_walls[wall].x1 < g_walls[wall].x2) {
            fromX = g_walls[wall].x1;
            toX = g_walls[wall].x2;
        } else {
            fromX = g_walls[wall].x2;
            toX = g_walls[wall].x1;
        }

        if (g_walls[wall].y1 < g_walls[wall].y2) {
            fromY = g_walls[wall].y1;
            toY = g_walls[wall].y2;
        } else {
            fromY = g_walls[wall].y2;
            toY = g_walls[wall].y1;
        }

        for (int i = fromX; i <= toX; i++) {
            for (int j = fromY; j <= toY; j++) {
                pixel(i, j, wallColor);
            }
        }
    }

    int playerColor[3] = {255, 255, 0};
    pixel(g_player.x, g_player.y, playerColor);
    glutSwapBuffers();
}

void setWallData(char *line, int lineNumber) {
    char *lineNext;
    g_walls[lineNumber].x1 = strtol(line, &lineNext, 10);
    g_walls[lineNumber].y1 = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].x2 = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].y2 = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].c[0] = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].c[1] = strtol(lineNext, &lineNext, 10);
    g_walls[lineNumber].c[2] = strtol(lineNext, &lineNext, 10);

}

void fileRead() {
    FILE *fp;
    char line[100];
    size_t len = 0;
    int lineNumber = 0;

    fp = fopen("./map", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp)) {
        setWallData(line, lineNumber);
        lineNumber++;
    }

    g_tree = createTree();
    for (int wall = 0; wall < sizeof(g_walls) / sizeof(g_walls[0]); wall++) {
        insert_node(&g_tree->root, g_walls[wall].x1, g_walls[wall].y1, g_walls[wall].x2, g_walls[wall].y2);
    }

    print_tree(g_tree->root);
    
    fclose(fp);
}

void initialization() {
    timer();
    fileRead();

    g_player.x = 50;
    g_player.y = 50;
    g_player.rotation = 0;

    g_pressedKeys.w = 0;
    g_pressedKeys.s = 0;
    g_pressedKeys.a = 0;
    g_pressedKeys.d = 0;
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(GLUT_WINDOW_POS_X, GLUT_WINDOW_POS_Y);
    glutInitWindowSize(GLUT_WINDOW_SIZE_WIDTH, GLUT_WINDOW_SIZE_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);


    g_window[0] = glutCreateWindow("Goom");
    glPointSize(1);
    gluOrtho2D(0, GLUT_WINDOW_SIZE_WIDTH, 0, GLUT_WINDOW_SIZE_HEIGHT);

    initialization();

    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    glutInitWindowPosition(GLUT_WINDOW_POS_X + GLUT_WINDOW_SIZE_WIDTH, GLUT_WINDOW_POS_Y);
    g_window[1] = glutCreateWindow("Overhead");
    glutDisplayFunc(overhead);

    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        printf("GLEW error: %s\n", glewGetErrorString(glewStatus));
        exit(EXIT_FAILURE);
    }

    gluOrtho2D(-GLUT_WINDOW_SIZE_WIDTH / 2, GLUT_WINDOW_SIZE_WIDTH / 2, -GLUT_WINDOW_SIZE_HEIGHT / 2,
               GLUT_WINDOW_SIZE_HEIGHT / 2);

    // Initialize the VBO and line vertices
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float startX = 100.0f;
    float startY = 100.0f;
    float endX = 200.0f;
    float endY = 200.0f;
    float lineVertices[] = {startX, startY, endX, endY};
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);

    // Enable vertex arrays and set up the vertex pointer
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);


    glutMainLoop();
    return 0;
}

