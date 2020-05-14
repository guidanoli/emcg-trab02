#pragma once

#include <vector>
#include <memory>

#include "igraphics.h"

class GraphicsController : public IGraphics
{
public:
	void addGraphics(std::shared_ptr<IGraphics> graphics)
	{
		m_graphics_vector.push_back(graphics);
	}
	void plot() override
	{
		for (auto const& graphics : m_graphics_vector)
			graphics->plot();
	}
private:
	std::vector<std::shared_ptr<IGraphics>> m_graphics_vector;
};