#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "structs.h"

#define DegToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define RadToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

#define GLUT_WINDOW_POS_Y 100
#define GLUT_WINDOW_POS_X 100

#define GLUT_WINDOW_SIZE_WIDTH 640
#define GLUT_WINDOW_SIZE_HEIGHT 480

#define PLAYER_SPEED 1

player g_player;
key g_pressedKeys;

void pixel(int x, int y, int rgb[3]) {
    glColor3ub(rgb[0], rgb[1], rgb[2]);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void clearScreen() {
    int color[3] = {0, 60, 130};
    int x, y;
    for (x = 0; x < GLUT_WINDOW_SIZE_WIDTH; x++) {
        for (y = 0; y < GLUT_WINDOW_SIZE_HEIGHT; y++) {
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
    double dx = cos(DegToRad(g_player.rotation)) * PLAYER_SPEED;
    double dy = sin(DegToRad(g_player.rotation)) * PLAYER_SPEED;

    if (g_pressedKeys.w == 1) {
        g_player.x += dx;
        g_player.y += dy;
    }
    if (g_pressedKeys.s == 1) {
        g_player.x -= dx;
        g_player.y -= dy;
    }
    if (g_pressedKeys.a == 1) {
        g_player.rotation -= 1 * PLAYER_SPEED;
        if (g_player.rotation < 0) {
            g_player.rotation = 360;
        }
    }
    if (g_pressedKeys.d == 1) {
        g_player.rotation += 1 * PLAYER_SPEED;
        if (g_player.rotation > 359) {
            g_player.rotation = 0;
        }
    }
}

void timer() {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clearScreen();

    move();

    printf("pos: (%f, %f) rot: %i\n", g_player.x, g_player.y, g_player.rotation);

    glutSwapBuffers();
}

void initialization() {
    timer();

    g_player.x = 0.0;
    g_player.y = 0.0;
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
    glutCreateWindow("goom?");
    glPointSize(1);
    gluOrtho2D(0, GLUT_WINDOW_SIZE_WIDTH, 0, GLUT_WINDOW_SIZE_HEIGHT);

    initialization();

    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
    return 0;
}

