#pragma once

#include <memory>

#include "mousecontroller.h"
#include "igraphics.h"

class Game;
enum class Cell;

class GBoard : public IGraphics, public IMouseListener
{
public:
	GBoard(std::shared_ptr<Game> game) :
		m_game(game),
		m_point_size(5.f),
		m_disc_points(32),
		m_disc_fill(0.8f),
		m_x(0), m_y(0), m_l(100),
		m_r(0.2f), m_g(0.2f), m_b(1.f),
		m_is_holding_piece(false)
	{
		std::fill(m_held_piece_indices, m_held_piece_indices + 2, 0);
		std::fill(m_held_piece_pos_ini, m_held_piece_pos_ini + 2, 0.f);
		std::fill(m_held_piece_pos_fin, m_held_piece_pos_fin + 2, 0.f);
		std::fill(m_placing_piece_pos, m_placing_piece_pos + 2, 0.f);
	}

	// Setters
	void setPointSize(float point_size) { m_point_size = point_size; }
	void setDiscPoints(int disc_points) { m_disc_points = disc_points; }
	void setDiscFill(float disc_fill) { m_disc_fill = disc_fill; }
	void setBoardPosition(float x, float y) { m_x = x; m_y = y; }
	void setBoardLength(float l) { m_l = l; }
	void setBoardColor(float r, float g, float b) { m_r = r; m_g = g; m_b = b; }

	// IGraphics
	void config() override;
	void plot() override;

	// IMouseListener
	void drag_cb(float x, float y) override;
	void move_cb(float x, float y) override;
	void click_cb(int button, int state, float x, float y) override;
private:
	void plotCircle(float cx, float cy, float r);
	void getCellCenter(int i, int j, float* c);
	
	// returns true if there is a piece in the cell
	// and false otherwise
	bool setPlayerColor(Cell color) const;
private:
	std::shared_ptr<Game> m_game;

	// mouse controller
	bool m_is_holding_piece;
	int m_held_piece_indices[2];
	float m_held_piece_pos_ini[2];
	float m_held_piece_pos_fin[2];

	float m_placing_piece_pos[2];

	// plotting parameters
	float m_point_size;
	int m_disc_points;
	float m_disc_fill;
	float m_r, m_g, m_b;
	float m_x, m_y, m_l;
};