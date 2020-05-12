#pragma once

class IMouseListener
{
public:
	virtual ~IMouseListener() {}
	virtual void click_cb(int button, int state, float x, float y) = 0;
	virtual void move_cb(float x, float y) = 0;
	virtual void drag_cb(float x, float y) = 0;
};