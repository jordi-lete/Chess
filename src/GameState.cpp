#include "GameState.h"
#include <iostream>

// Constructor
GameState::GameState()
{

	m_isWhiteTurn = true;
	m_showMoves = false;
	gameOver = false;
	moveMade = false;
	promotionInProgress = false;
	m_isCapture = false;
	m_isCheck = false;
	m_isCastling = false;

}

bool GameState::makeMove(Board& board, Move& move)
{
	move.capturedPiece = board.getPieceAt(move.endFile, move.endRank);
	move.ps_enPassantTarget = board.lastDoublePawnMove;
	handleEnPassant(board, move.movingPiece, move.startFile, move.startRank, move.endFile, move.endRank, &move);
	m_isCapture = (move.capturedPiece != Board::NONE) ? true : false;
	m_isCastling = move.isCastling;


	if (handleCastling(board, move.movingPiece, move.startFile, move.startRank, move.endFile, move.endRank, &move))
	{
		// play castling sound
	}

	if (move.isPromotion)
	{
		board.squares[move.endFile][move.endRank] = move.promotionPiece;
	}
	else
	{
		board.squares[move.endFile][move.endRank] = move.movingPiece;
	}

	board.squares[move.startFile][move.startRank] = Board::NONE;

	// check if the opponent king in check and play check sound?
	// else play regular move sound

	m_isWhiteTurn = !m_isWhiteTurn;
	// Do this after switching turns so we check if the opponent is in check
	m_isCheck = isInCheck(board);
	moveMade = true;

	return true;
}

void GameState::unmakeMove(Board& board, const Move& move)
{
	// move the piece back to it's start position
	board.squares[move.startFile][move.startRank] = move.movingPiece;

	if (move.isEnPassant)
	{
		board.squares[move.endFile][move.endRank] = Board::NONE;
		board.squares[move.endFile][move.startRank] = move.capturedPiece;
	}
	else if (move.isCastling)
	{
		board.squares[move.endFile][move.endRank] = Board::NONE;
		
		int rookStartFile = (move.startFile < move.endFile) ? 7 : 0;
		int rookEndFile = (move.startFile < move.endFile) ? 5 : 3;

		// set the rook start square
		board.squares[rookStartFile][move.endRank] = board.squares[rookEndFile][move.endRank];
		// set the rook end squard back to none
		board.squares[rookEndFile][move.endRank] = Board::NONE;
	}
	else
	{
		board.squares[move.endFile][move.endRank] = move.capturedPiece;
	}

	board.whiteKingMoved = move.ps_whiteKingMoved;
	board.blackKingMoved = move.ps_blackKingMoved;
	board.whiteRookKSMoved = move.ps_whiteRookKSMoved;
	board.whiteRookQSMoved = move.ps_whiteRookQSMoved;
	board.blackRookKSMoved = move.ps_blackRookKSMoved;
	board.blackRookQSMoved = move.ps_blackRookQSMoved;
	board.lastDoublePawnMove = move.ps_enPassantTarget;

	m_isWhiteTurn = !m_isWhiteTurn;
}

std::vector<Move> GameState::generateAllLegalMoves(Board& board)
{
	std::vector<Move> legalMoves;

	for (int startFile = 0; startFile < 8; startFile++)
	{
		for (int startRank = 0; startRank < 8; startRank++)
		{
			Board::PieceType piece = board.getPieceAt(startFile, startRank);
			if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn)
			{
				continue;
			}
			Piece* validator = m_Validator.getValidator(piece);
			std::vector<Square> possibleMoves = validator->getPossibleMoves(board, startFile, startRank);
			std::vector<Square> pieceLegalMoves = returnLegalMoves(board, possibleMoves, piece, startFile, startRank);

			for (const auto& dest : pieceLegalMoves)
			{
				Move move;
				move.startFile = startFile;
				move.startRank = startRank;
				move.endFile = dest.file;
				move.endRank = dest.rank;
				move.movingPiece = piece;
				move.isEnPassant = ((piece == Board::WHITE_PAWN || piece == Board::BLACK_PAWN) && dest.file != startFile && board.squares[dest.file][dest.rank] == board.NONE);
				move.isCastling = ((piece == Board::WHITE_KING || piece == Board::BLACK_KING) && std::abs(dest.file - startFile) == 2);
				bool isWhite = board.getPieceColour(piece);
				int promotionSquare = isWhite ? 0 : 7;
				move.isPromotion = ((piece == Board::WHITE_PAWN || piece == Board::BLACK_PAWN) && dest.rank == promotionSquare);

				Move tempMove = move;

				if (move.isPromotion)
				{
					Board::PieceType promotionPieces[4] = {
						isWhite ? Board::WHITE_KNIGHT : Board::BLACK_KNIGHT,
						isWhite ? Board::WHITE_BISHOP : Board::BLACK_BISHOP,
						isWhite ? Board::WHITE_ROOK : Board::BLACK_ROOK,
						isWhite ? Board::WHITE_QUEEN : Board::BLACK_QUEEN
					};
					for (auto& promotionPiece : promotionPieces)
					{
						tempMove.promotionPiece = promotionPiece;
						legalMoves.push_back(tempMove);
					}
				}
				else
				{
					legalMoves.push_back(move);
				}
			}
		}
	}

	return legalMoves;
}

bool GameState::tryMakeMove(Board& board, int startFile, int startRank, int endFile, int endRank, Board::PieceType promotionPiece)
{

	Board::PieceType piece = board.getPieceAt(startFile, startRank);
	Board::PieceType targetPiece = board.getPieceAt(endFile, endRank);
	// Check if it is the turn of the piece clicked
	if (piece == Board::NONE || board.getPieceColour(piece) != m_isWhiteTurn)
	{
		m_showMoves = false;
		m_Moves.clear();
		return false;
	}
	// This is here to fix minor bug where possible moves would show if a piece was dragged onto friendly piece
	if (targetPiece != Board::NONE && board.getPieceColour(targetPiece) == m_isWhiteTurn)
	{
		m_showMoves = false;
		m_Moves.clear();
		return false;
	}
	// This is here to fix minor bug where possible moves would show if a piece was dragged onto friendly piece
	if (targetPiece != Board::NONE && board.getPieceColour(targetPiece) == m_isWhiteTurn)
	{
		m_showMoves = false;
		m_Moves.clear();
		return false;
	}

	std::vector<Move> legalMoves = generateAllLegalMoves(board);
	
	for (auto& move : legalMoves)
	{
		if (move.startFile == startFile && move.startRank == startRank && move.endFile == endFile && move.endRank == endRank)
		{
			// Handle promotion first as the player may decide to cancel
			move.promotionPiece = promotionPiece;
			if (handlePromotion(board, move))
			{
				m_Moves.clear();
				m_showMoves = false;
				return false;
			}

			makeMove(board, move);

			// check is that move was checkmate
			gameOver = isCheckmate(board);

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

/* ----------------- Get all possible moves for a piece in the current position ----------------- */

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
	std::vector<Square> possibleMoves = validator->getPossibleMoves(board, startFile, startRank);
	std::vector<Square> legalMoves = returnLegalMoves(board, possibleMoves, piece, startFile, startRank);
	
	//Store the moves in a variable to be accessed by the renderer
	m_Moves = legalMoves;
	m_showMoves = true;

}

std::vector<Square> GameState::returnLegalMoves(Board& board, std::vector<Square> moves, Board::PieceType piece, int startFile, int startRank)
{
	std::vector<Square> legalMoves;

	// If the move would leave the king in check it is not valid
	for (auto& move : moves)
	{
		// Make the move on the fake board first
		Board fakeBoard = board;
		// If the move is castling then check the in between move is not in check either
		if ((piece == board.WHITE_KING || piece == board.BLACK_KING) && std::abs(startFile - move.file) == 2)
		{
			int direction = (move.file - startFile) / 2;
			if (isAttacked(board, startFile + direction, move.rank) || isAttacked(board, startFile, startRank))
			{
				continue;
			}
		}
		handleEnPassant(fakeBoard, piece, startFile, startRank, move.file, move.rank);
		fakeBoard.squares[move.file][move.rank] = piece;
		fakeBoard.squares[startFile][startRank] = board.NONE;
		if (!isInCheck(fakeBoard))
		{
			legalMoves.push_back({ move.file, move.rank });
		}
	}

	return legalMoves;
}

/* ----------------- Utility Functions ----------------- */

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

/* -------------- Special Cases -------------- */

void GameState::handleEnPassant(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank, Move* move)
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
		if (move != nullptr)
		{
			move->isEnPassant = true;
			move->capturedPiece = board.getPieceAt(endFile, startRank);	
		}
		board.squares[endFile][startRank] = board.NONE;
	}
}

bool GameState::handleCastling(Board& board, Board::PieceType piece, int startFile, int startRank, int endFile, int endRank, Move* move)
{
	bool castling = false;
	// We want to track the previous state of the board so we can later undo
	if (move != nullptr)
	{
		move->ps_whiteKingMoved = board.whiteKingMoved;
		move->ps_whiteRookKSMoved = board.whiteRookKSMoved;
		move->ps_whiteRookQSMoved = board.whiteRookQSMoved;
		move->ps_blackKingMoved = board.blackKingMoved;
		move->ps_blackRookKSMoved = board.blackRookKSMoved;
		move->ps_blackRookQSMoved = board.blackRookQSMoved;
	}

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
			castling = true;
			if (move != nullptr)
			{
				move->isCastling = true;
			}
			// Move the rook here only
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
	return castling;
}

bool GameState::handlePromotion(Board& board, Move& move)
{

	if (move.isPromotion)
	{
		if (move.promotionPiece == Board::NONE)
		{
			m_promotionData.endFile = move.endFile;
			m_promotionData.endRank = move.endRank;
			m_promotionData.startFile = move.startFile;
			m_promotionData.startRank = move.startRank;
			promotionInProgress = true;
		}
		return true;
	}
	return false; // Non promotion move played
}

void GameState::completePromotion(Board& board, Board::PieceType promotionPiece)
{
	if (promotionPiece != Board::NONE)
	{
		Move promotionMove;
		promotionMove.startFile = m_promotionData.startFile;
		promotionMove.startRank = m_promotionData.startRank;
		promotionMove.endFile = m_promotionData.endFile;
		promotionMove.endRank = m_promotionData.endRank;
		promotionMove.isPromotion = true;
		promotionMove.promotionPiece = promotionPiece;
		makeMove(board, promotionMove);
	}
	promotionInProgress = false;
}

/* ----------------- Check if king is attacked ----------------- */

bool GameState::isInCheck(Board& board)
{
	Board::PieceType king = m_isWhiteTurn ? board.WHITE_KING : board.BLACK_KING;
	Square kingPos = { -1, -1 };
	// iterate through each square on the board to find king	
	for (int f = 0; f < 8; f++)
	{
		for (int r = 0; r < 8; r++)
		{
			if (board.getPieceAt(f, r) == king)
			{
				kingPos = { f, r };
				break;
			}
		}
		if (kingPos.file >= 0) { break; }
	}

	// Now we check if any of the opposition pieces are attacking the kingPos
	return isAttacked(board, kingPos.file, kingPos.rank);
}


bool GameState::isCheckmate(Board& board)
{
	for (int f = 0; f < 8; f++)
	{
		for (int r = 0; r < 8; r++)
		{
			Board::PieceType piece = board.getPieceAt(f, r);
			if (piece == board.NONE || board.getPieceColour(piece) != m_isWhiteTurn)
			{
				continue;
			}
			else
			{
				Piece* validator = m_Validator.getValidator(piece);
				std::vector<Square> moves = validator->getPossibleMoves(board, f, r);
				std::vector<Square> legalMoves = returnLegalMoves(board, moves, piece, f, r);
				if (!legalMoves.empty())
				{
					return false;
				}
			}
		}
	}
	return true;
}

/* ----------------- Check if a square is under attack ----------------- */

bool GameState::isAttacked(Board& board, int file, int rank)
{
	// Now we check if any of the opposition pieces are attacking the kingPos
	for (int f = 0; f < 8; f++)
	{
		for (int r = 0; r < 8; r++)
		{
			Board::PieceType piece = board.getPieceAt(f, r);
			if (piece == board.NONE || board.getPieceColour(piece) == m_isWhiteTurn)
			{
				continue;
			}
			else
			{
				Piece* validator = m_Validator.getValidator(piece);
				std::vector<Square> pieceMoves = validator->getPossibleMoves(board, f, r);
				for (auto& move : pieceMoves)
				{
					if (move.file == file && move.rank == rank)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* ------------ Retrieve private booleans ------------ */

bool GameState::getIsCheck()
{
	return m_isCheck;
}

bool GameState::getIsCapture()
{
	return m_isCapture;
}

bool GameState::getIsCastling()
{
	return m_isCastling;
}
