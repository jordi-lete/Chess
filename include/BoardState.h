#pragma once

struct Square
{
	int file;
	int rank;
	Square() : file(0), rank(0) {}
	Square(int f, int r) : file(f), rank(r) {}
};

class Board
{
public:
	Square lastDoublePawnMove;
	bool blackKingMoved;
	bool whiteKingMoved;
	bool blackRookKSMoved;
	bool whiteRookKSMoved;
	bool blackRookQSMoved;
	bool whiteRookQSMoved;

	// From 0 - 12
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
		if (file < 8 && rank < 8 && file >= 0 && rank >= 0)
		{
			return squares[file][rank];
		}
		return NONE;
	}
	
	// returns true if white and false if black
	bool getPieceColour(PieceType piece);

	bool isValidPosition(int file, int rank);

private:
	void setUpBoardStartPos();

};
