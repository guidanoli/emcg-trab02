#pragma once

#include <memory>
#include <random>

class Board;
enum class Cell;

class Game
{
public:
	enum class Stage
	{
		PLACING_PIECES,
		PLAYING,
		END,
	};
public:
	Game(int dim, std::default_random_engine& rng);
	std::shared_ptr<Board const> getBoard() const;

	Cell getTurn() const;
	Stage getStage() const;

	bool canPlacePieces() const;
	bool canMovePieces() const;

	bool placePiece(int i, int j);
	bool movePiece(int i_ini, int j_ini, int i_fin, int j_fin);
private:
	void nextTurn();
	void addPlacedPieces();
	void processMove(int i, int j);
	Cell getEnemy(Cell me) const;
	void eliminateCell(Cell& cell);
	bool isCentralCell(int i, int j) const;
	bool hasPossibleMove(Cell player) const;
private:
	std::shared_ptr<Board> m_board;
	int m_yellow_pieces, m_red_pieces;
	int m_remaining_pieces_to_place;
	Stage m_stage;
	Cell m_turn;
};