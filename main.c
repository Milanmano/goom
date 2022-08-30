#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

#define GLUT_WINDOW_POS_Y 100
#define GLUT_WINDOW_POS_X 100

#define GLUT_WINDOW_SIZE_WIDTH 640
#define GLUT_WINDOW_SIZE_HEIGHT 480

void pixel(int x, int y, int rgb[3]) {
    glColor3ub(rgb[0], rgb[1], rgb[2]);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int color[3] = {0, 60, 130};

    int x, y;
    for (x = 0; x < GLUT_WINDOW_SIZE_WIDTH; x++) {
        for (y = 0; y < GLUT_WINDOW_SIZE_HEIGHT; y++) {
            pixel(x, y, color);
        }
    }

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
    glutMainLoop();
    return 0;
}

