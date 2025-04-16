#include "GameState.h"

// Constructor
GameState::GameState()
{

	m_isWhiteTurn = true;
	m_showMoves = false;

}

bool GameState::tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank)
{

	Board::PieceType piece = board.getPieceAt(startFile, startRank);
	// Check if it is the turn of the piece clicked
	if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn) 
	{
		// PLAY A SOUND OR DISPLAY ALERT?
		m_showMoves = false;
		m_Moves.clear();
		return false;
	}

	// Find the correct set of methods for that piece
	Piece* validator = m_Validator.getValidator(piece);

	std::vector<Square> legalMoves = validator->getPossibleMoves(board, startFile, startRank);
	
	for (auto& move : legalMoves)
	{
		if (move.file == endFile && move.rank == endRank)
		{
			board.squares[endFile][endRank] = piece;
			board.squares[startFile][startRank] = board.NONE;
			m_isWhiteTurn = !m_isWhiteTurn;
			m_Moves.clear();
			m_showMoves = false;
			return true;
		}
	}
	m_Moves.clear();
	m_showMoves = false;
	getPossibleMoves(board, endFile, endRank);
	return false;

}

void GameState::getPossibleMoves(Board& board, int startFile, int startRank)
{

	Board::PieceType piece = board.getPieceAt(startFile, startRank);
	// Check if it is the turn of the piece clicked
	if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn)
	{
		// PLAY A SOUND OR DISPLAY ALERT?
		m_Moves.clear();
		m_showMoves = false;
		return;
	}

	// Find the correct set of methods for that piece
	Piece* validator = m_Validator.getValidator(piece);
	std::vector<Square> legalMoves = validator->getPossibleMoves(board, startFile, startRank);

	//Store the moves in a variable to be accessed by the renderer
	m_Moves = legalMoves;
	m_showMoves = true;

}

bool GameState::getCurrentTurn()
{
	return m_isWhiteTurn;
}

const bool GameState::showMoves() const
{
	return m_showMoves;
}

const std::vector<Square>& GameState::getMoves() const 
{
	return m_Moves;
}