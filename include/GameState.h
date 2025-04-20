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

	bool gameOver;

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank);

	void getPossibleMoves(Board& board, int startFile, int startRank);

	std::vector<Square> returnLegalMoves(Board& board, std::vector<Square> moves, Board::PieceType piece, int startFile, int startRank);

	bool getCurrentTurn();

	const bool showMoves() const;

	const std::vector<Square>& GameState::getMoves() const;

	void handleEnPassant(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank);

	void handleCastling(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRAnk);

	bool isInCheck(Board& board);

	bool isCheckmate(Board& board);

};