#include "board.h"

#include <cassert>

int main(int argc, char** argv)
{
	Board board(5);
	assert(board.getDimension() == 5);
	
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			assert(board[i][j] == Cell::EMPTY);

	return 0;
}