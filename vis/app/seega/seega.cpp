#include <iostream>

#include <GL/glut.h>

#include "graphicscontroller.h"
#include "gboard.h"
#include "mousecontroller.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define WINDOW_PROJ_WIDTH 100.f
#define WINDOW_PROJ_HEIGHT 100.f
#define WINDOW_MARGIN 10.f

std::unique_ptr<GraphicsController> gcontroller_ptr = nullptr;
std::unique_ptr<MouseController> mcontroller_ptr = nullptr;

void display()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (gcontroller_ptr)
		gcontroller_ptr->plot();
	glutSwapBuffers();
}

void click(int button, int state, int x, int y)
{
	if (mcontroller_ptr)
		mcontroller_ptr->click_cb(button, state, x, y);
}

void move(int x, int y)
{
	if (mcontroller_ptr)
		mcontroller_ptr->move_cb(x, y);
}

int main(int argc, char** argv)
{
	gcontroller_ptr = std::make_unique<GraphicsController>();
	mcontroller_ptr = std::make_unique<MouseController>(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		-WINDOW_MARGIN,
		WINDOW_PROJ_WIDTH + WINDOW_MARGIN,
		WINDOW_PROJ_HEIGHT + WINDOW_MARGIN,
		-WINDOW_MARGIN
	);

	//mcontroller_ptr->setDebug(true);

	auto board_ptr = std::make_shared<Board>(5);

	(*board_ptr)[0][0] = Cell::RED;
	(*board_ptr)[2][3] = Cell::YELLOW;

	auto gboard_ptr = std::make_shared<GBoard>(board_ptr);
	gcontroller_ptr->addGraphics(gboard_ptr);
	mcontroller_ptr->addListener(gboard_ptr);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Seega");
	glutDisplayFunc(display);
	glutMouseFunc(click);
	glutMotionFunc(move);
	gluOrtho2D(
		- WINDOW_MARGIN,
		(double) WINDOW_PROJ_WIDTH + WINDOW_MARGIN,
		(double) WINDOW_PROJ_HEIGHT + WINDOW_MARGIN,
		- WINDOW_MARGIN
	);
	glutMainLoop();
}