#include <GL/freeglut.h>
#include <stdio.h>

static void RenderSceneCB() {
	static GLclampf c = 0.0f;
	glClearColor(c, c, c, c);
	c += 1.0f/256.0f;
	if (c >= 1.0f) {
		c = 0.0f;
	}
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	int width = 1920;
	int height = 1080;
	glutInitWindowSize(width, height);

	int x = 200;
	int y = 100;
	glutInitWindowPosition(x, y);
	int win = glutCreateWindow("Tutorial 01");
	printf("window id: %d\n", win);

	GLclampf Red = 1.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}

// use -display to specify the X server