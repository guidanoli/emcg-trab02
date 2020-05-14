#include "gboard.h"

#include <thread>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include <GL/glut.h>

#include "game.h"
#include "board.h"


using namespace std::chrono_literals;

void GBoard::plot()
{
	/* Board */
	auto m_board = m_game->getBoard();
	const int dim = m_board->getDimension();
	const float div = m_l / dim;
	glColor3f(m_r, m_g, m_b);
	glPointSize(m_point_size);
	for (int i = 0; i <= dim; ++i) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= dim; ++j)
			glVertex2f(m_x + i * div, m_y + j * div);
		glEnd();
	}
	for (int i = 0; i <= dim; ++i) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= dim; ++j)
			glVertex2f(m_x + j * div, m_y + i * div);
		glEnd();
	}
	/* Cells */
	int const* last_move = m_game->getLastMove();
	auto const& last_removed = m_game->getLastRemoved();
	const float r = div * m_disc_fill / 2;
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j) {
			if (m_is_holding_piece &&
				m_held_piece_indices[0] == i &&
				m_held_piece_indices[1] == j)
				continue; // skip held piece
			if (m_ai_is_animating &&
				last_move[2] == i &&
				last_move[3] == j)
				continue; // skip animating piece
			if (m_ai_is_animating) {
				bool was_last_removed = false;
				for (auto const& [rem_i, rem_j] : last_removed)
					if (rem_i == i && rem_j == j) {
						was_last_removed = true;
						break;
					}
				if (was_last_removed)
					continue; // skip removed pieces
			}
			if (setPlayerColor((*m_board)[i][j])) {
				float c[2];
				getCellCenter(i, j, c);
				plotCircle(c[0], c[1], r);
			}
		}
	/* Special cells */
	if (m_is_holding_piece) {
		float c[2];
		int i = m_held_piece_indices[0];
		int j = m_held_piece_indices[1];
		getCellCenter(i, j, c);
		c[0] += m_held_piece_pos_fin[0] - m_held_piece_pos_ini[0];
		c[1] += m_held_piece_pos_fin[1] - m_held_piece_pos_ini[1];
		if (setPlayerColor((*m_board)[i][j]))
			plotCircle(c[0], c[1], r);
	}
	if (m_game->getStage() == Game::Stage::PLACING_PIECES &&
		!m_game->isAiTurn()) {
		Cell piece_color = m_game->getTurn();
		if (setPlayerColor(piece_color))
			plotCircle(m_placing_piece_pos[0], m_placing_piece_pos[1], r);
	}
	if (m_ai_is_animating) {
		auto now = std::chrono::steady_clock::now();
		unsigned long long dt =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(now - m_ai_animation_start).count();
		float progress;
		if (dt >= m_ai_animation_duration) {
			progress = 1.f;
			m_ai_is_animating = false;
		} else {
			progress = (float) dt / (float) m_ai_animation_duration;
		}
		Cell piece_color = m_game->getTurn();
		if (progress != 1.f) {
			setPlayerColor(piece_color);
			for (auto const& [rem_i, rem_j] : last_removed) {
				float c[2];
				getCellCenter(rem_i, rem_j, c);
				plotCircle(c[0], c[1], r);
			}
		}
		float last_positions[4];
		getCellCenter(last_move[0], last_move[1], last_positions);
		getCellCenter(last_move[2], last_move[3], last_positions + 2);
		float curr_x = last_positions[0] * (1.f - progress) + last_positions[2] * progress;
		float curr_y = last_positions[1] * (1.f - progress) + last_positions[3] * progress;
		setPlayerColor(m_game->getAiColor());
		plotCircle(curr_x, curr_y, r);
		if (progress < 1.f)
			glutPostRedisplay();
	}
	if (m_game->isAiTurn()) {
		m_game->letAiPlay();
		if (m_ai_animate && m_game->getStage() == Game::Stage::PLAYING) {
			m_ai_is_animating = true;
			m_ai_animation_start = std::chrono::steady_clock::now();
		}
		glutPostRedisplay();
	}
}

void GBoard::plotCircle(float cx, float cy, float r)
{
	/* Parametric plot */
	double t = 0;
	const double dt = 2 * M_PI / m_disc_points;
	glBegin(GL_POLYGON);
	for (int n = 0; n < m_disc_points; ++n, t += dt) {
		double x = r * cos(t) + cx;
		double y = r * sin(t) + cy;
		glVertex2d(x, y);
	}
	glEnd();
}

bool GBoard::setPlayerColor(Cell color) const
{
	switch (color) {
	case Cell::EMPTY:
		return false;
	case Cell::YELLOW:
		glColor3f(1.f, 1.f, 0.f);
		break;
	case Cell::RED:
		glColor3f(1.f, 0.f, 0.f);
		break;
	}
	return true;
}

void GBoard::drag_cb(float x, float y)
{
	if (m_game->getStage() == Game::Stage::PLAYING &&
		m_is_holding_piece) {
		m_held_piece_pos_fin[0] = x;
		m_held_piece_pos_fin[1] = y;
		glutPostRedisplay();
	}
}

void GBoard::move_cb(float x, float y)
{
	if (m_game->getStage() == Game::Stage::PLACING_PIECES) {
		m_placing_piece_pos[0] = x;
		m_placing_piece_pos[1] = y;
		glutPostRedisplay();
	}
}

void GBoard::getCellCenter(int i, int j, float* c)
{
	auto m_board = m_game->getBoard();
	const int dim = m_board->getDimension();
	const float div = m_l / dim;
	c[0] = m_x + j * div + div / 2;
	c[1] = m_y + i * div + div / 2;
}

void GBoard::click_cb(int button, int state, float x, float y)
{
	if (button == GLUT_LEFT_BUTTON) {
		auto m_board = m_game->getBoard();
		const int dim = m_board->getDimension();
		const float div = m_l / dim;
		if (state == GLUT_DOWN) {
			int i = (int) ((y - m_y) / div);
			int j = (int) ((x - m_x) / div);
			if (i < 0 || i >= dim || j < 0 || j >= dim)
				return; // Off the board
			float cx, cy, dx, dy, r;
			switch (m_game->getStage()) {
			case Game::Stage::PLAYING:
				if ((*m_board)[i][j] == Cell::EMPTY)
					return; // Empty Cell
				cx = j * div + m_x + div / 2;
				cy = i * div + m_y + div / 2;
				dx = cx - x;
				dy = cy - y;
				r = div * m_disc_fill / 2;
				if (dx * dx + dy * dy > r * r)
					return; // Off the piece radius
				m_held_piece_indices[0] = i;
				m_held_piece_indices[1] = j;
				m_held_piece_pos_ini[0] = m_held_piece_pos_fin[0] = x;
				m_held_piece_pos_ini[1] = m_held_piece_pos_fin[1] = y;
				m_is_holding_piece = true;
				break;
			case Game::Stage::PLACING_PIECES:
				if (!m_game->placePiece(i, j))
					return;
				break;
			default:
				return;
			}
		} else {
			if (!m_is_holding_piece)
				return;
			int old_i = m_held_piece_indices[0];
			int old_j = m_held_piece_indices[1];
			float c[2];
			getCellCenter(old_i, old_j, c);
			c[0] += m_held_piece_pos_fin[0] - m_held_piece_pos_ini[0];
			c[1] += m_held_piece_pos_fin[1] - m_held_piece_pos_ini[1];
			if (c[0] >= m_x && c[0] <= m_x + m_l &&
				c[1] >= m_y && c[1] <= m_y + m_l) {
				int i = (int) ((c[1] - m_y) / div);
				int j = (int) ((c[0] - m_x) / div);
				m_game->movePiece(old_i, old_j, i, j);
			}
			m_is_holding_piece = false;
		}
		glutPostRedisplay();
	}
}