#pragma once
#include "BoardState.h"

class GameState
{

private:
	bool m_isWhiteTurn;

public:
	// Constructor
	GameState();

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank);

};