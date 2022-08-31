#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "structs.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

#define GLUT_WINDOW_POS_Y 100
#define GLUT_WINDOW_POS_X 100

#define GLUT_WINDOW_SIZE_WIDTH 640
#define GLUT_WINDOW_SIZE_HEIGHT 480

player P;

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
            printf("forward\n");
            break;
        case 's':
            printf("backward\n");
            break;
        case 'a':
            printf("turn left\n");
            break;
        case 'd':
            printf("turn right\n");
            break;
        default:
            printf("not mapped key\n");
            break;
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clearScreen();


    glutSwapBuffers();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(GLUT_WINDOW_POS_X, GLUT_WINDOW_POS_Y);
    glutInitWindowSize(GLUT_WINDOW_SIZE_WIDTH, GLUT_WINDOW_SIZE_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("goom?");

    glPointSize(1);
    gluOrtho2D(0, GLUT_WINDOW_SIZE_WIDTH, 0, GLUT_WINDOW_SIZE_HEIGHT);

    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

