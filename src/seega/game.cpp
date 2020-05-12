#include "game.h"

#include <iostream>

#include "board.h"

Game::Game(int dim, std::default_random_engine& rng) :
	m_board(std::make_shared<Board>(dim)),
	m_turn(rng() % 2 == 0 ? Cell::YELLOW : Cell::RED),
	m_stage(Stage::PLACING_PIECES),
	m_remaining_pieces_to_place(2),
	m_yellow_pieces(0),
	m_red_pieces(0)
{}

Cell Game::getTurn() const
{
	return m_turn;
}

std::shared_ptr<Board const> Game::getBoard() const
{
	return m_board;
}

Game::Stage Game::getStage() const
{
	return m_stage;
}

bool Game::canMovePieces() const
{
	return m_stage == Game::Stage::PLAYING;
}

bool Game::canPlacePieces() const
{
	return m_stage == Game::Stage::PLACING_PIECES;
}

bool Game::isCentralCell(int i, int j) const
{
	const int dim = m_board->getDimension();
	return i == dim / 2 && j == dim / 2;
}

bool Game::placePiece(int i, int j)
{
	if (!canPlacePieces())
		return false;
	const int dim = m_board->getDimension();
	if (i < 0 || i >= dim || j < 0 || j >= dim)
		return false; // Invalid indices
	if (isCentralCell(i,j))
		return false; // Central cell
	Cell& cell = (*m_board)[i][j];
	if (cell != Cell::EMPTY)
		return false;
	cell = m_turn;
	addPlacedPieces();
	if (--m_remaining_pieces_to_place == 0 &&
		m_stage == Game::Stage::PLACING_PIECES) {
		m_remaining_pieces_to_place = 2;
		nextTurn();
	}
	return true;
}

bool Game::movePiece(int i_ini, int j_ini, int i_fin, int j_fin)
{
	if (!canMovePieces())
		return false;
	const int dim = m_board->getDimension();
	if (i_ini < 0 || i_ini >= dim || j_ini < 0 || j_ini >= dim ||
		i_fin < 0 || i_fin >= dim || j_fin < 0 || j_fin >= dim)
		return false; // Invalid indices
	if (std::abs(i_ini - i_fin) + std::abs(j_ini - j_fin) != 1)
		return false; // Invalid move
	Cell& cell_ini = (*m_board)[i_ini][j_ini];
	if (cell_ini != m_turn)
		return false;
	Cell& cell_fin = (*m_board)[i_fin][j_fin];
	if (cell_fin != Cell::EMPTY)
		return false; // Tried to move piece to not empty cell
	std::swap(cell_ini, cell_fin);
	processMove(i_fin, j_fin);
	if (hasPossibleMove(getEnemy(m_turn)))
		nextTurn(); // If enemy player doesn't have move, keep the current one
	return true;
}

bool Game::hasPossibleMove(Cell player) const
{
	const int dim = m_board->getDimension();
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j)
			if ((*m_board)[i][j] == Cell::EMPTY) {
				if (i > 0 && (*m_board)[(std::size_t) i - 1][j] == player)
					return true;
				if (i < dim - 1 && (*m_board)[(std::size_t) i + 1][j] == player)
					return true;
				if (j > 0 && (*m_board)[i][(std::size_t) j - 1] == player)
					return true;
				if (j < dim - 1 && (*m_board)[i][(std::size_t) j + 1] == player)
					return true;
			}
	return false;
}

void Game::processMove(int i, int j)
{
	const int dim = m_board->getDimension();
	Cell cell = (*m_board)[i][j];
	// North
	if (i >= 2 && !isCentralCell(i - 1, j)) {
		Cell& north_cell = (*m_board)[(std::size_t) i - 1][j];
		Cell north_north_cell = (*m_board)[(std::size_t) i - 2][j];
		if (north_north_cell == cell && north_cell == getEnemy(cell))
			eliminateCell(north_cell);
	}
	// South
	if (i <= dim - 3 && !isCentralCell(i + 1, j)) {
		Cell& south_cell = (*m_board)[(std::size_t) i + 1][j];
		Cell south_south_cell = (*m_board)[(std::size_t) i + 2][j];
		if (south_south_cell == cell && south_cell == getEnemy(cell))
			eliminateCell(south_cell);
	}
	// West
	if (j >= 2 && !isCentralCell(i, j - 1)) {
		Cell& west_cell = (*m_board)[i][(std::size_t) j - 1];
		Cell west_west_cell = (*m_board)[i][(std::size_t) j - 2];
		if (west_west_cell == cell && west_cell == getEnemy(cell))
			eliminateCell(west_cell);
	}
	// East
	if (j <= dim - 3 && !isCentralCell(i, j + 1)) {
		Cell& east_cell = (*m_board)[i][(std::size_t) j + 1];
		Cell east_east_cell = (*m_board)[i][(std::size_t) j + 2];
		if (east_east_cell == cell && east_cell == getEnemy(cell))
			eliminateCell(east_cell);
	}
}

void Game::eliminateCell(Cell& cell)
{
	switch (cell) {
	case Cell::YELLOW:
		--m_yellow_pieces;
		break;
	case Cell::RED:
		--m_red_pieces;
		break;
	default:
		return;
	}
	cell = Cell::EMPTY;
}

void Game::nextTurn()
{
	m_turn = getEnemy(m_turn);
}

Cell Game::getEnemy(Cell me) const
{
	return me == Cell::RED ? Cell::YELLOW : Cell::RED;
}

void Game::addPlacedPieces()
{
	if (m_stage != Game::Stage::PLACING_PIECES)
		return;
	if (m_turn == Cell::YELLOW)
		++m_yellow_pieces;
	else
		++m_red_pieces;
	const int dim = m_board->getDimension();
	if ((m_red_pieces + m_yellow_pieces) == ((dim * dim) - 1))
		m_stage = Game::Stage::PLAYING;
}