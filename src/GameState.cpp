#include "GameState.h"
#include <iostream>

// Constructor
GameState::GameState()
{

	m_isWhiteTurn = true;

}

bool GameState::tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank)
{

	Board::PieceType piece = board.getPieceAt(startFile, startRank);
	// Check if it is the turn of the piece clicked
	if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn) 
	{
		// PLAY A SOUND OR DISPLAY ALERT?
		return false;
	}

	// Find the correct set of methods for that piece
	Piece* validator = m_Validator.getValidator(piece);

	// Case when piece is clicked rather than dragged
	if (startFile == endFile && startRank == endRank)
	{
		// ADD SOMETHING TO SHOW VALID MOVES
		std::vector<Square> moves = validator->getPossibleMoves(board, startFile, startRank);
		for (auto move : moves)
		{
			std::cout << "possible move: " << move.file << ", " << move.rank << std::endl;
		}
		return false;
	}

	//std::vector<Square> moves = validator->getPossibleMoves(board, startFile, startRank);
	
	// ADD SOME isValidMove FUNCTION HERE

	board.squares[endFile][endRank] = piece;
	board.squares[startFile][startRank] = board.NONE;
	m_isWhiteTurn = !m_isWhiteTurn;
	
	return true;

}
