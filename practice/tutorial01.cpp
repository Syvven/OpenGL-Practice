

static void InitializeGlutCallbacks() {
    glutDisplayFunc(RenderSceneCB);
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

	GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}

// use -display to specify the X server