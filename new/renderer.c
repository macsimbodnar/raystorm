#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "renderer.h"

#define D_WIDTH     1024
#define D_HEIGHT    512
#define WINDOW_X    300
#define WINDOW_Y    1200


void init() {
    glutInit(0, 0);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(D_WIDTH, D_HEIGHT);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutCreateWindow("RAY STORM");

    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, D_WIDTH, D_HEIGHT, 0);

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(button_down);
    glutKeyboardUpFunc(button_up);
}