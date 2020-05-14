#pragma once

class IGraphics
{
public:
	virtual ~IGraphics() {}
	virtual void plot() = 0;
};