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
			handleEnPassant(board, piece, startFile, startRank, endFile, endRank);
			handleCastling(board, piece, startFile, startRank, endFile, endRank);

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

void GameState::handleEnPassant(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank)
{
	// First reset pawn move, this is overwritten if the move is a double pawn push
	board.lastDoublePawnMove = { -1, -1 };
	// Check if the move was a double pawn push
	if ((piece == Board::WHITE_PAWN || piece == Board::BLACK_PAWN) && std::abs(startRank - endRank) == 2)
	{
		board.lastDoublePawnMove = { endFile, endRank };
	}
	// Now check if it was an En Passant capture
	else if ((piece == Board::WHITE_PAWN || piece == Board::BLACK_PAWN) && endFile != startFile && board.squares[endFile][endRank] == board.NONE)
	{
		board.squares[endFile][startRank] = board.NONE;
	}
}

void GameState::handleCastling(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank)
{
	// If rook or king moved, we can no longer castle
	if (piece == Board::WHITE_ROOK)
	{
		if (startFile == 0 && startRank == 7) { board.whiteRookQSMoved = true; }
		if (startFile == 7 && startRank == 7) { board.whiteRookKSMoved = true; }
	}
	else if (piece == Board::BLACK_ROOK)
	{
		if (startFile == 0 && startRank == 0) { board.blackRookQSMoved = true; }
		if (startFile == 7 && startRank == 0) { board.blackRookKSMoved = true; }
	}
	else if (piece == Board::WHITE_KING || piece == Board::BLACK_KING)
	{
		if (std::abs(startFile - endFile) == 2) // We are castling
		{
			if (endFile == 2) // Queenside
			{
				board.squares[3][endRank] = board.squares[0][endRank];
				board.squares[0][endRank] = board.NONE;
			}
			else if (endFile == 6) // Kingside
			{
				board.squares[5][endRank] = board.squares[7][endRank];
				board.squares[7][endRank] = board.NONE;
			}
		}
		if (piece == Board::WHITE_KING) { board.whiteKingMoved = true; }
		else { board.blackKingMoved = true; }
	}

	// Now check if either rook is captured
	Board::PieceType targetPiece = board.getPieceAt(endFile, endRank);
	if (targetPiece == Board::WHITE_ROOK)
	{
		if (endFile == 0 && endRank == 7)
		{
			board.whiteRookQSMoved = true;
		}
		else if (endFile == 7 && endRank == 7)
		{
			board.whiteRookKSMoved = true;
		}
	}
	else if (targetPiece == Board::BLACK_ROOK)
	{
		if (endFile == 0 && endRank == 0)
		{
			board.blackRookQSMoved = true;
		}
		else if (endFile == 7 && endRank == 0)
		{
			board.blackRookKSMoved = true;
		}
	}
}

