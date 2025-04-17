#pragma once
#include "Piece.h"

class Pawn : public Piece
{
private:
	void checkForEnPassant(Board& board, int file, int rank, bool isWhite, std::vector<Square>& moves);

public:
	std::vector<Square> getPossibleMoves(Board& board, int file, int rank);

};