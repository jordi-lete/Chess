#pragma once
#include "BoardState.h"
#include <vector>

class Piece 
{
public:
	virtual ~Piece() = default;
	virtual std::vector<Square> getPossibleMoves(Board& board, int file, int rank) = 0;

};