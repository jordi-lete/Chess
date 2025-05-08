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

struct Move
{
	int startFile = -1;
	int startRank = -1;
	int endFile = -1;
	int endRank = -1;
	Board::PieceType movingPiece = Board::NONE;
	Board::PieceType capturedPiece = Board::NONE;
	bool isCastling = false;
	bool isEnPassant = false;
	bool isPromotion = false;
	Board::PieceType promotionPiece = Board::NONE;

	// The following are needed for undoing moves ( where ps = previous state )
	Square ps_enPassantTarget = { -1,-1 };
	bool ps_whiteKingMoved = false;
	bool ps_blackKingMoved = false;
	bool ps_whiteRookKSMoved = false;
	bool ps_whiteRookQSMoved = false;
	bool ps_blackRookKSMoved = false;
	bool ps_blackRookQSMoved = false;
};

class GameState
{

private:
	bool m_isWhiteTurn;
	Validator m_Validator;
	std::vector<Square> m_Moves;
	bool m_showMoves;
	Promotion m_promotionData;
	bool m_isCheck;
	bool m_isCapture;
	bool m_isCastling;
	int m_evaluation;

public:
	// Constructor
	GameState();

	bool gameOver;
	bool moveMade;

	bool promotionInProgress;

	bool makeMove(Board& board, Move& move);

	void unmakeMove(Board& board, const Move& move);

	std::vector<Move> generateAllLegalMoves(Board& board);

	bool tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank, Board::PieceType promotionPiece = Board::NONE);

	void getPossibleMoves(Board& board, int startFile, int startRank);

	std::vector<Square> returnLegalMoves(Board& board, std::vector<Square> moves, Board::PieceType piece, int startFile, int startRank);

	bool getCurrentTurn();

	const bool showMoves() const;

	const std::vector<Square>& GameState::getMoves() const;

	void handleEnPassant(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank, Move* move = nullptr);

	bool handleCastling(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank, Move* move = nullptr);

	bool handlePromotion(Board& board, Move& move);

	void completePromotion(Board& board, Board::PieceType promotionPiece);

	bool isInCheck(Board& board);

	bool isCheckmate(Board& board);

	bool isAttacked(Board& board, int file, int rank);

	bool getIsCheck();
	bool getIsCapture();
	bool getIsCastling();

};