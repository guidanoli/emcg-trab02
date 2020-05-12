#include "gboard.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <GL/glut.h>

void GBoard::config()
{
	// nothing
}

void GBoard::plot()
{
	/* Board */
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
	const float r = div * m_disc_fill / 2;
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j) {
			if (m_is_holding_piece &&
				m_held_piece_indices[0] == i &&
				m_held_piece_indices[1] == j)
				continue; // skip held piece
			switch ((*m_board)[i][j]) {
			case Cell::EMPTY:
				continue; // draw nothing
			case Cell::YELLOW:
				glColor3f(1.f, 1.f, 0.f);
				break;
			case Cell::RED:
				glColor3f(1.f, 0.f, 0.f);
				break;
			}
			float c[2];
			getCellCenter(i, j, c);
			plotCircle(c[0], c[1], r);
		}
	if (m_is_holding_piece) {
		float c[2];
		int i = m_held_piece_indices[0];
		int j = m_held_piece_indices[1];
		getCellCenter(i, j, c);
		c[0] += m_held_piece_pos_fin[0] - m_held_piece_pos_ini[0];
		c[1] += m_held_piece_pos_fin[1] - m_held_piece_pos_ini[1];
		switch ((*m_board)[i][j]) {
		case Cell::EMPTY:
			return;
		case Cell::YELLOW:
			glColor3f(1.f, 1.f, 0.f);
			break;
		case Cell::RED:
			glColor3f(1.f, 0.f, 0.f);
			break;
		}
		plotCircle(c[0], c[1], r);
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

void GBoard::move_cb(float x, float y)
{
	if (m_is_holding_piece) {
		m_held_piece_pos_fin[0] = x;
		m_held_piece_pos_fin[1] = y;
		glutPostRedisplay();
	}
}

void GBoard::getCellCenter(int i, int j, float* c)
{
	const int dim = m_board->getDimension();
	const float div = m_l / dim;
	c[0] = m_x + j * div + div / 2;
	c[1] = m_y + i * div + div / 2;
}

void GBoard::click_cb(int button, int state, float x, float y)
{
	if (button == GLUT_LEFT_BUTTON) {
		const int dim = m_board->getDimension();
		const float div = m_l / dim;
		if (state == GLUT_DOWN) {
			int i = (int) ((y - m_y) / div);
			int j = (int) ((x - m_x) / div);
			if (i < 0 || i >= dim || j < 0 || j >= dim)
				return; // Off the board
			if ((*m_board)[i][j] == Cell::EMPTY)
				return; // Empty Cell
			const float cx = j * div + m_x + div / 2;
			const float cy = i * div + m_y + div / 2;
			const float dx = cx - x;
			const float dy = cy - y;
			const float r = div * m_disc_fill / 2;
			if (dx * dx + dy * dy > r * r)
				return; // Off the piece radius
			m_held_piece_indices[0] = i;
			m_held_piece_indices[1] = j;
			m_held_piece_pos_ini[0] = m_held_piece_pos_fin[0] = x;
			m_held_piece_pos_ini[1] = m_held_piece_pos_fin[1] = y;
			m_is_holding_piece = true;
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
				if ((*m_board)[i][j] == Cell::EMPTY)
					// Hard-coded --> should consult logic classes
					std::swap((*m_board)[i][j], (*m_board)[old_i][old_j]);
			}
			m_is_holding_piece = false;
		}
		glutPostRedisplay();
	}
}