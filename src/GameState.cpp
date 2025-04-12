#include "GameState.h"

// Constructor
GameState::GameState()
{

	m_isWhiteTurn = true;

}

bool GameState::tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank)
{

	Board::PieceType piece = board.getPieceAt(startFile, startRank);
	if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn) 
	{
		return false;
	}

	// ADD SOME isValidMove FUNCTION HERE

	board.squares[endFile][endRank] = piece;
	board.squares[startFile][startRank] = board.NONE;
	m_isWhiteTurn = !m_isWhiteTurn;
	
	return true;
}
