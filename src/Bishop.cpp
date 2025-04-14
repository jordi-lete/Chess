#include "Bishop.h"

std::vector<Square> Bishop::getPossibleMoves(Board& board, int file, int rank)
{
	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));
	int newFile = file;
	int newRank = rank;

	// Check 4 directions until we find a piece or board edge
	// First - positive x and y
	while (true)
	{

		newFile++;
		newRank++;

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE)
			{
				moves.push_back({ newFile, newRank });
				continue;
			}
			else if (board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

	}

	newFile = file;
	newRank = rank;

	//  positive x, negative y
	while (true)
	{

		newFile++;
		newRank--;

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE)
			{
				moves.push_back({ newFile, newRank });
				continue;
			}
			else if (board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

	}

	newFile = file;
	newRank = rank;

	// negative x, positive y
	while (true)
	{

		newFile--;
		newRank++;

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE)
			{
				moves.push_back({ newFile, newRank });
				continue;
			}
			else if (board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

	}

	newFile = file;
	newRank = rank;

	// negative x and y
	while (true)
	{

		newFile--;
		newRank--;

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE)
			{
				moves.push_back({ newFile, newRank });
				continue;
			}
			else if (board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

	}

	return moves;

}