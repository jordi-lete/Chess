#pragma once
#include "BoardState.h"
#include "Validator.h"

struct Promotion
{
	int endFile;
	int endRank;
	int startFile;
	int startRank;
	Promotion() : endFile(-1), endRank(-1), startFile(-1), startRank(-1) {}
};

class GameState
{

private:
	bool m_isWhiteTurn;
	Validator m_Validator;
	std::vector<Square> m_Moves;
	bool m_showMoves;
	Promotion m_promotionData;
	bool m_success = false;

public:
	// Constructor
	GameState();

	bool gameOver;

	bool promotionInProgress;

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank);

	void getPossibleMoves(Board& board, int startFile, int startRank);

	std::vector<Square> returnLegalMoves(Board& board, std::vector<Square> moves, Board::PieceType piece, int startFile, int startRank);

	bool getCurrentTurn();

	const bool showMoves() const;

	const std::vector<Square>& GameState::getMoves() const;

	void handleEnPassant(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank);

	void handleCastling(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank);

	bool handlePromotion(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank);

	void completePromotion(Board& board, Board::PieceType promotionPiece);

	bool isInCheck(Board& board);

	bool isCheckmate(Board& board);

	bool isAttacked(Board& board, int file, int rank);

};