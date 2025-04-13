#pragma once
#include "Piece.h"

class Bishop : public Piece
{

public:
	std::vector<Square> getPossibleMoves(Board& board, int file, int rank);

};