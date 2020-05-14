#include "game.h"

#include <iostream>
#include <numeric>

#include "board.h"

Game::Game(int dim, bool ai, std::default_random_engine& rng) :
	m_board(std::make_shared<Board>(dim)),
	m_turn(rng() % 2 == 0 ? Cell::YELLOW : Cell::RED),
	m_stage(Stage::PLACING_PIECES),
	m_remaining_pieces_to_place(2),
	m_yellow_pieces(0),
	m_red_pieces(0),
	m_ai(ai),
	m_rng(rng)
{
	std::fill(m_last_move, m_last_move + 4, 0);
	m_ai_turn = getEnemy(m_turn);
}

Cell Game::getTurn() const
{
	return m_turn;
}

Cell Game::getAiColor() const
{
	return m_ai_turn;
}

std::shared_ptr<Board const> Game::getBoard() const
{
	return m_board;
}

Game::Stage Game::getStage() const
{
	return m_stage;
}

bool Game::isOver() const
{
	return m_stage == Game::Stage::END;
}

bool Game::canMovePieces() const
{
	return m_stage == Game::Stage::PLAYING && !isAiTurn();
}

int const* Game::getLastMove() const
{
	return m_last_move;
}

bool Game::canPlacePieces() const
{
	return m_stage == Game::Stage::PLACING_PIECES && !isAiTurn();
}

bool Game::isAiTurn() const
{
	return m_ai && m_turn == m_ai_turn && !isOver();
}

std::vector<std::pair<int, int>> const& Game::getLastRemoved() const
{
	return m_last_removed;
}

bool Game::isCentralCell(int i, int j) const
{
	const int dim = m_board->getDimension();
	return i == dim / 2 && j == dim / 2;
}

bool Game::letAiPlay()
{
	if (!isAiTurn())
		return false;
	switch (m_stage) {
	case Game::Stage::PLACING_PIECES:
		return chooseCellToPlace();
	case Game::Stage::PLAYING:
		return chooseMove();
	default:
		return false;
	}
}

bool Game::chooseCellToPlace()
{
	int i_ = 0, j_ = 0;
	const int dim = m_board->getDimension();
	const int piece_cnt = (m_turn == Cell::RED)
		? m_red_pieces : m_yellow_pieces;
	float ic = 0.f, jc = 0.f;
	if (piece_cnt == 0) {
		ic = jc = (float) dim / 2;
	} else {
		for (int i = 0; i < dim; ++i)
			for (int j = 0; j < dim; ++j)
				if ((*m_board)[i][j] == m_turn) {
					ic += (float) i / piece_cnt;
					jc += (float) j / piece_cnt;
				}
	}
	std::vector<std::pair<int, int>> empty_spaces;
	std::vector<float> cell_dists;
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j)
			if ((*m_board)[i][j] == Cell::EMPTY && !isCentralCell(i, j)) {
				float di = (float) i - ic;
				float dj = (float) j - jc;
				float dist = di * di + dj * dj;
				empty_spaces.push_back(std::make_pair(i, j));
				cell_dists.push_back(dist);
				i_ = i;
				j_ = j;
			}
	float dist_sum = std::accumulate(cell_dists.begin(), cell_dists.end(), 0.f);
	auto unif = std::uniform_real_distribution<float>();
	float u = unif(m_rng);
	float x = 0.f;
	for (std::size_t i = 0; i < cell_dists.size(); ++i) {
		if (u - x <= 1E-6f) {
			auto chosen = empty_spaces[i];
			i_ = chosen.first;
			j_ = chosen.second;
			break;
		}
		x += cell_dists[i] / dist_sum;
	}
	return placePiecePrivate(i_, j_);
}

bool Game::chooseMove()
{
	const int dim = m_board->getDimension();
	std::vector<std::tuple<int, int, int, int>> moves;
	for (int i = 0; i < dim; ++i)
		for (int j = 0; j < dim; ++j)
			if ((*m_board)[i][j] == Cell::EMPTY) {
				if (i > 0 && (*m_board)[(std::size_t) i - 1][j] == m_turn)
					moves.push_back(std::make_tuple(i - 1, j, i, j));
				if (j > 0 && (*m_board)[i][(std::size_t) j - 1] == m_turn)
					moves.push_back(std::make_tuple(i, j - 1, i, j));
				if (i < dim - 1 && (*m_board)[(std::size_t) i + 1][j] == m_turn)
					moves.push_back(std::make_tuple(i + 1, j, i, j));
				if (j < dim - 1 && (*m_board)[i][(std::size_t) j + 1] == m_turn)
					moves.push_back(std::make_tuple(i, j + 1, i, j));
			}
	const auto enemy = getEnemy(m_turn);
	for (auto const& [i_ini, j_ini, i_fin, j_fin] : moves) {
		if (i_fin >= 2 && !isCentralCell(i_fin - 1, j_fin)) {
			Cell& north_cell = (*m_board)[(std::size_t) i_fin - 1][j_fin];
			Cell north_north_cell = (*m_board)[(std::size_t) i_fin - 2][j_fin];
			if (north_north_cell == m_turn && north_cell == enemy)
				return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
		}
		// South
		if (i_fin <= dim - 3 && !isCentralCell(i_fin + 1, j_fin)) {
			Cell& south_cell = (*m_board)[(std::size_t) i_fin + 1][j_fin];
			Cell south_south_cell = (*m_board)[(std::size_t) i_fin + 2][j_fin];
			if (south_south_cell == m_turn && south_cell == enemy)
				return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
		}
		// West
		if (j_fin >= 2 && !isCentralCell(i_fin, j_fin - 1)) {
			Cell& west_cell = (*m_board)[i_fin][(std::size_t) j_fin - 1];
			Cell west_west_cell = (*m_board)[i_fin][(std::size_t) j_fin - 2];
			if (west_west_cell == m_turn && west_cell == enemy)
				return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
		}
		// East
		if (j_fin <= dim - 3 && !isCentralCell(i_fin, j_fin + 1)) {
			Cell& east_cell = (*m_board)[i_fin][(std::size_t) j_fin + 1];
			Cell east_east_cell = (*m_board)[i_fin][(std::size_t) j_fin + 2];
			if (east_east_cell == m_turn && east_cell == enemy)
				return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
		}
	}
	decltype(moves) chosen_move(1);
	std::sample(moves.begin(), moves.end(), chosen_move.begin(), 1, m_rng);
	auto const& [i_ini, j_ini, i_fin, j_fin] = chosen_move[0];
	return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
}

bool Game::placePiece(int i, int j)
{
	if (!canPlacePieces())
		return false;
	return placePiecePrivate(i, j);
}

bool Game::placePiecePrivate(int i, int j)
{
	const int dim = m_board->getDimension();
	if (i < 0 || i >= dim || j < 0 || j >= dim)
		return false; // Invalid indices
	if (isCentralCell(i, j))
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
	return movePiecePrivate(i_ini, j_ini, i_fin, j_fin);
}

bool Game::movePiecePrivate(int i_ini, int j_ini, int i_fin, int j_fin)
{
	const int dim = m_board->getDimension();

	// Check validity of move
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

	// Save last mvoe
	m_last_move[0] = i_ini;
	m_last_move[1] = j_ini;
	m_last_move[2] = i_fin;
	m_last_move[3] = j_fin;

	// Process move
	std::swap(cell_ini, cell_fin);
	processMove(i_fin, j_fin);
	if (m_red_pieces == 0) {
		std::cout << "Yellow won!\n";
		m_stage = Game::Stage::END;
	} else if (m_yellow_pieces == 0) {
		std::cout << "Red won!\n";
		m_stage = Game::Stage::END;
	} else {
		// If enemy player doesn't have move, keep the current one
		if (hasPossibleMove(getEnemy(m_turn)))
			nextTurn();
	}
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

	// Clear last removed
	m_last_removed.clear();

	// North
	if (i >= 2 && !isCentralCell(i - 1, j)) {
		Cell& north_cell = (*m_board)[(std::size_t) i - 1][j];
		Cell north_north_cell = (*m_board)[(std::size_t) i - 2][j];
		if (north_north_cell == cell && north_cell == getEnemy(cell))
			eliminateCell(i - 1, j);
	}
	// South
	if (i <= dim - 3 && !isCentralCell(i + 1, j)) {
		Cell& south_cell = (*m_board)[(std::size_t) i + 1][j];
		Cell south_south_cell = (*m_board)[(std::size_t) i + 2][j];
		if (south_south_cell == cell && south_cell == getEnemy(cell))
			eliminateCell(i + 1, j);
	}
	// West
	if (j >= 2 && !isCentralCell(i, j - 1)) {
		Cell& west_cell = (*m_board)[i][(std::size_t) j - 1];
		Cell west_west_cell = (*m_board)[i][(std::size_t) j - 2];
		if (west_west_cell == cell && west_cell == getEnemy(cell))
			eliminateCell(i, j - 1);
	}
	// East
	if (j <= dim - 3 && !isCentralCell(i, j + 1)) {
		Cell& east_cell = (*m_board)[i][(std::size_t) j + 1];
		Cell east_east_cell = (*m_board)[i][(std::size_t) j + 2];
		if (east_east_cell == cell && east_cell == getEnemy(cell))
			eliminateCell(i, j + 1);
	}
}

void Game::eliminateCell(int i, int j)
{
	m_last_removed.push_back(std::make_pair(i, j));

	auto& cell = (*m_board)[i][j];
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
	if ((m_red_pieces + m_yellow_pieces) == ((dim * dim) - 1)) {
		m_stage = Game::Stage::PLAYING;
		if (!hasPossibleMove(m_turn))
			nextTurn();
	}
}