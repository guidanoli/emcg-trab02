#include "board.h"

#include <cassert>

Board::Board(int dim) :
	m_dim(dim)
{
	assert(dim > 0);
	for (int i = 0; i < dim; ++i) {
		std::vector<Cell> line(dim);
		std::fill(line.begin(), line.end(), Cell::EMPTY);
		m_cells.push_back(line);
	}
}

std::vector<Cell> const& Board::operator[](std::size_t i) const
{
	assert(i >= 0);
	assert(i < m_dim);
	return m_cells[i];
}

std::vector<Cell>& Board::operator[](std::size_t i)
{
	assert(i >= 0);
	assert(i < m_dim);
	return m_cells[i];
}

int Board::getDimension() const
{
	return m_dim;
}