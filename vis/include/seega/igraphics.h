#pragma once

class IGraphics
{
public:
	virtual ~IGraphics() {}
	virtual void plot() = 0;
	virtual void config() = 0;
};