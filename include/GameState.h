#pragma once
#include "BoardState.h"
#include "Validator.h"

class GameState
{

private:
	bool m_isWhiteTurn;
	Validator m_Validator;

public:
	// Constructor
	GameState();

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank);

};