#pragma once

#include <vector>

enum class Cell
{
	EMPTY,
	YELLOW,
	RED
};

class Board
{
public:
	Board(int dim);
	int getDimension() const;
	std::vector<Cell> const& operator[](std::size_t i) const;
	std::vector<Cell>& operator[](std::size_t i);
private:
	int m_dim;
	std::vector<std::vector<Cell>> m_cells;
};