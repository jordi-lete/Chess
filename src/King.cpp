#include "King.h"

std::vector<Square> King::getPossibleMoves(Board& board, int file, int rank)
{
	
	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));

	const int kingMoves[8][2] = {
		{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}
	};

	for (int i = 0; i < 8; i++)
	{
		int newFile = file + kingMoves[i][0];
		int newRank = rank + kingMoves[i][1];

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE || board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
			}
		}
	}

	int castleRank = isWhite ? 7 : 0;
	if (canCastle(board, isWhite, true)) // if can castle kingside
	{
		moves.push_back({ 6, castleRank });
	}
	if (canCastle(board, isWhite, false)) // if can castle queenside
	{
		moves.push_back({ 2, castleRank });
	}

	return moves;

}

bool King::canCastle(Board& board, bool isWhite, bool kingSide)
{

	if (isWhite && board.whiteKingMoved || !isWhite && board.blackKingMoved)
	{
		return false;
	}

	int rank = isWhite ? 7 : 0;

	if (kingSide)
	{
		if (isWhite && board.whiteRookKSMoved || !isWhite && board.blackRookKSMoved)
		{
			return false;
		}
		if (board.squares[5][rank] != board.NONE || board.squares[6][rank] != board.NONE)
		{
			return false;
		}
	}

	else // Queenside
	{
		if (isWhite && board.whiteRookQSMoved || !isWhite && board.blackRookQSMoved)
		{
			return false;
		}
		if (board.squares[3][rank] != board.NONE || board.squares[2][rank] != board.NONE || board.squares[1][rank] != board.NONE)
		{
			return false;
		}
	}
	return true;

}