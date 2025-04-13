#pragma once
#include "Piece.h"

class Rook : public Piece
{

public:
	std::vector<Square> getPossibleMoves(Board& board, int file, int rank);

};