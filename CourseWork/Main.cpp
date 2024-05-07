#include <GL/glut.h>
#include "GLOBAL.h"
#include "Vertex.h"
#include "Shape.h"

//Vars
float z = 0, ax = 0, ay = 0;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Vertex tPoints[] = {
    Vertex( - 1.0f,-1.0f,-1.0f), // triangle 1 : begin
    Vertex( - 1.0f,-1.0f, 1.0f),
    Vertex(-1.0f, 1.0f, 1.0f), // triangle 1 : end
    Vertex(1.0f, 1.0f,-1.0f), // triangle 2 : begin
    Vertex(-1.0f,-1.0f,-1.0f),
    Vertex(-1.0f, 1.0f,-1.0f), // triangle 2 : end
    Vertex(1.0f,-1.0f, 1.0f),
    Vertex(-1.0f,-1.0f,-1.0f),
    Vertex(1.0f,-1.0f,-1.0f),
    Vertex(1.0f, 1.0f,-1.0f),
    Vertex(1.0f,-1.0f,-1.0f),
    Vertex(-1.0f,-1.0f,-1.0f),
    Vertex(-1.0f,-1.0f,-1.0f),
    Vertex(-1.0f, 1.0f, 1.0f),
    Vertex(-1.0f, 1.0f,-1.0f),
    Vertex(1.0f,-1.0f, 1.0f),
    Vertex(-1.0f,-1.0f, 1.0f),
    Vertex(-1.0f,-1.0f,-1.0f),
    Vertex(-1.0f, 1.0f, 1.0f),
    Vertex(-1.0f,-1.0f, 1.0f),
    Vertex(1.0f,-1.0f, 1.0f),
    Vertex(1.0f, 1.0f, 1.0f),
    Vertex(1.0f,-1.0f,-1.0f),
    Vertex(1.0f, 1.0f,-1.0f),
    Vertex(1.0f,-1.0f,-1.0f),
    Vertex(1.0f, 1.0f, 1.0f),
    Vertex(1.0f,-1.0f, 1.0f),
    Vertex(1.0f, 1.0f, 1.0f),
    Vertex(1.0f, 1.0f,-1.0f),
    Vertex(-1.0f, 1.0f,-1.0f),
    Vertex(1.0f, 1.0f, 1.0f),
    Vertex(-1.0f, 1.0f,-1.0f),
    Vertex(-1.0f, 1.0f, 1.0f),
    Vertex(1.0f, 1.0f, 1.0f),
    Vertex(-1.0f, 1.0f, 1.0f),
    Vertex(1.0f,-1.0f, 1.0f)
    };
	Vertex* tPointStart = &tPoints[0];
	Shape triangle(tPointStart, sizeof(tPoints) / sizeof(tPoints[0]));
	triangle.rotate(ax, ay);
	triangle.draw();
	glFlush();
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'w') {
		ax -= 0.1;
	}
	else if (key == 's') {
		ax += 0.1;
	}
	else if (key == 'd') {
		ay += 0.1;
	}
	else if (key == 'a') {
		ay -= 0.1;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	ShowWindow(GetConsoleWindow(), SW_HIDE); //Hide console
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(INITIAL_WINDOW_X, INITIAL_WINDOW_Y);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("GL");
	glClearColor(0.0, 0.0, 0.0, 0.0); //Black backgroud
	glMatrixMode(GL_PROJECTION); //Setup viewing projection
	glLoadIdentity(); //Start with identity matrix
	glOrtho(0.0, WIDTH, 0.0, HEIGHT, -DEPTH, 0.0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyPressed);
	glTranslatef(WIDTH / 2, HEIGHT / 2, 0);
	glutMainLoop();
	return 0;
}