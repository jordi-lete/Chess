#pragma once

class Board
{
public:
	// From 0 - 13
	enum PieceType
	{
		NONE, WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
		BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
	};
	// initialise a board array
	PieceType squares[8][8];

	// Constructor
	Board();

	PieceType getPieceAt(int file, int rank) const
	{
		return squares[file][rank];
	}

private:
	void setUpBoardStartPos();

};