#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include <GL/glut.h>

#include "imouselistener.h"

class MouseController
{
public:
	MouseController(int screen_width, int screen_height,
		float left, float right, float bottom, float top) :
		m_screen_height(screen_height),
		m_screen_width(screen_width),
		m_left(left),
		m_right(right),
		m_bottom(bottom),
		m_top(top)
	{}
	void addListener(std::shared_ptr<IMouseListener> listener)
	{
		m_listeners.push_back(listener);
	}
	void click_cb(int button, int state, int x, int y)
	{
		if (!m_listen_click)
			return;
		float proj_x = ((float) x / m_screen_width) * (m_right - m_left) + m_left;
		float proj_y = ((float) y / m_screen_height) * (m_bottom - m_top) + m_top;
		for (auto const& listener : m_listeners)
			listener->click_cb(button, state, proj_x, proj_y);
		if (m_debug) {
			std::cerr << "[DEBUG] Event ";
			switch (button) {
			case GLUT_LEFT_BUTTON:
				std::cerr << "left";
				break;
			case GLUT_RIGHT_BUTTON:
				std::cerr << "right";
				break;
			case GLUT_MIDDLE_BUTTON:
				std::cerr << "middle";
				break;
			default:
				std::cerr << '?';
				break;
			}
			std::cerr << '_';
			switch (state) {
			case GLUT_UP:
				std::cerr << "up";
				break;
			case GLUT_DOWN:
				std::cerr << "down";
				break;
			default:
				std::cerr << '?';
				break;
			}
			std::cerr << " at (" << x << ',' << y << ") on screen or at ("
				<< proj_x << ',' << proj_y << ") in the world\n";
		}
	}
	void drag_cb(int x, int y)
	{
		if (!m_listen_drag)
			return;
		float proj_x = ((float) x / m_screen_width) * (m_right - m_left) + m_left;
		float proj_y = ((float) y / m_screen_height) * (m_bottom - m_top) + m_top;
		for (auto const& listener : m_listeners)
			listener->drag_cb(proj_x, proj_y);
		if (m_debug) {
			std::cerr << "[DEBUG] Event drag at (" << x << ',' << y
				<< ") on screen or at (" << proj_x << ',' << proj_y
				<< ") in the world\n";
		}
	}
	void move_cb(int x, int y)
	{
		if (!m_listen_move)
			return;
		float proj_x = ((float) x / m_screen_width) * (m_right - m_left) + m_left;
		float proj_y = ((float) y / m_screen_height) * (m_bottom - m_top) + m_top;
		for (auto const& listener : m_listeners)
			listener->move_cb(proj_x, proj_y);
		if (m_debug) {
			std::cerr << "[DEBUG] Event move at (" << x << ',' << y
				<< ") on screen or at (" << proj_x << ',' << proj_y
				<< ") in the world\n";
		}
	}
	void listenDrag(bool listen_drag) { m_listen_drag = listen_drag; }
	void listenMove(bool listen_move) { m_listen_move = listen_move; }
	void listenClick(bool listen_click) { m_listen_click = listen_click; }
	void setDebug(bool debug) { m_debug = debug; }
private:
	std::vector<std::shared_ptr<IMouseListener>> m_listeners;
	bool m_listen_click = true;
	bool m_listen_drag = true;
	bool m_listen_move = true;
	bool m_debug = false;
	float m_left, m_right, m_bottom, m_top;
	int m_screen_height, m_screen_width;
};