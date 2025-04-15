#pragma once
#include "BoardState.h"
#include "Validator.h"

class GameState
{

private:
	bool m_isWhiteTurn;
	Validator m_Validator;
	std::vector<Square> m_Moves;
	bool m_showMoves;

public:
	// Constructor
	GameState();

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank);

	const bool showMoves() const;

	const std::vector<Square>& GameState::getMoves() const;

};