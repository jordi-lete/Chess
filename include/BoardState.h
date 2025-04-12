#pragma once

struct Square
{
	int file;
	int rank;
	Square() : file(0), rank(0) {}
};

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
	
	// returns true if white and false if black
	bool getPieceColour(PieceType piece);

private:
	void setUpBoardStartPos();

};
