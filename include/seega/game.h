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
	Game(int dim, bool ai, std::default_random_engine& rng);
	std::shared_ptr<Board const> getBoard() const;

	Cell getTurn() const;
	Cell getAiColor() const;
	bool isAiTurn() const;

	bool letAiPlay();

	Stage getStage() const;
	bool isOver() const;

	bool canPlacePieces() const;
	bool canMovePieces() const;

	bool placePiece(int i, int j);
	bool movePiece(int i_ini, int j_ini, int i_fin, int j_fin);

	int const* getLastMove() const;
	std::vector<std::pair<int, int>> const& getLastRemoved() const;
private:
	bool placePiecePrivate(int i, int j);
	bool movePiecePrivate(int i_ini, int j_ini, int i_fin, int j_fin);

	void nextTurn();
	void addPlacedPieces();
	void processMove(int i, int j);
	Cell getEnemy(Cell me) const;
	void eliminateCell(int i, int j);
	bool isCentralCell(int i, int j) const;
	bool hasPossibleMove(Cell player) const;

	bool chooseCellToPlace();
	bool chooseMove();
private:
	std::shared_ptr<Board> m_board;
	int m_yellow_pieces, m_red_pieces;
	int m_remaining_pieces_to_place;
	std::default_random_engine m_rng;
	bool m_ai;
	std::vector<std::pair<int, int>> m_last_removed;
	int m_last_move[4];
	Stage m_stage;
	Cell m_turn;
	Cell m_ai_turn;
};