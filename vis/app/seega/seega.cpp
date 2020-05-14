#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <cerrno>

#include <GL/glut.h>

#include "argparser.h"

#include "game.h"
#include "board.h"

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

namespace arg = argparser;

std::string get_file_contents(std::string const& filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

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

void drag(int x, int y)
{
	if (mcontroller_ptr)
		mcontroller_ptr->drag_cb(x, y);
}

void move(int x, int y)
{
	if (mcontroller_ptr)
		mcontroller_ptr->move_cb(x, y);
}

struct options_t
{
	int board_size;
	bool ai_adversary;
};

int main(int argc, char** argv)
{
	options_t options;

	const std::string regras_path = std::string(DATAPATH) + "/regras.txt";
	const std::string regras_str = get_file_contents(regras_path);

	arg::build_parser(argc, argv, options, regras_str.c_str())

		.bind("tamanho", &options_t::board_size,
			arg::doc("Tamanho do tabuleiro"),
			arg::def(5))
		
		.bind("ia", &options_t::ai_adversary,
			arg::doc("Jogar contra adversario robo"),
			arg::def(true))

		.build();

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

	std::default_random_engine rng((unsigned int) time(NULL));
	auto game_ptr = std::make_shared<Game>(
		options.board_size,
		options.ai_adversary,
		rng);
	auto gboard_ptr = std::make_shared<GBoard>(game_ptr);
	gcontroller_ptr->addGraphics(gboard_ptr);
	mcontroller_ptr->addListener(gboard_ptr);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Seega");
	glutDisplayFunc(display);
	glutMouseFunc(click);
	glutMotionFunc(drag);
	glutPassiveMotionFunc(move);
	gluOrtho2D(
		- WINDOW_MARGIN,
		(double) WINDOW_PROJ_WIDTH + WINDOW_MARGIN,
		(double) WINDOW_PROJ_HEIGHT + WINDOW_MARGIN,
		- WINDOW_MARGIN
	);
	glutMainLoop();
}