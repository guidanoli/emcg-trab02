#include "board.h"

#include <cassert>

int main(int argc, char** argv)
{
	Board board(5);
	assert(board.getDimension() == 5);
	
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			assert(board[i][j] == Cell::EMPTY);

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j) {
			switch ((i + j) % 3) {
			case 0:
				board[i][j] = Cell::EMPTY;
				break;
			case 1:
				board[i][j] = Cell::YELLOW;
				break;
			case 2:
				board[i][j] = Cell::RED;
				break;
			}
		}

	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j) {
			switch ((i + j) % 3) {
			case 0:
				assert(board[i][j] == Cell::EMPTY);
				break;
			case 1:
				assert(board[i][j] == Cell::YELLOW);
				break;
			case 2:
				assert(board[i][j] == Cell::RED);
				break;
			}
		}

	return 0;
}