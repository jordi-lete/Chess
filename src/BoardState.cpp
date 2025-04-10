#include "BoardState.h"

// Constructor
Board::Board() 
{
	//Initialise an empty board
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			squares[file][rank] = NONE;
		}
	}
	setUpBoardStartPos();
}

void Board::setUpBoardStartPos()
{

	squares[0][0] = BLACK_ROOK;
	squares[1][0] = BLACK_KNIGHT;
	squares[2][0] = BLACK_BISHOP;
	squares[3][0] = BLACK_QUEEN;
	squares[4][0] = BLACK_KING;
	squares[5][0] = BLACK_BISHOP;
	squares[6][0] = BLACK_KNIGHT;
	squares[7][0] = BLACK_ROOK;

	squares[0][7] = WHITE_ROOK;
	squares[1][7] = WHITE_KNIGHT;
	squares[2][7] = WHITE_BISHOP;
	squares[3][7] = WHITE_QUEEN;
	squares[4][7] = WHITE_KING;
	squares[5][7] = WHITE_BISHOP;
	squares[6][7] = WHITE_KNIGHT;
	squares[7][7] = WHITE_ROOK;

	for (int i = 0; i < 8; i++)
	{
		squares[i][1] = BLACK_PAWN;
		squares[i][6] = WHITE_PAWN;
	}

}