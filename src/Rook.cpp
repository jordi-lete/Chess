#include "Rook.h"

std::vector<Square> Rook::getPossibleMoves(Board& board, int file, int rank)
{
	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));
	const int directions[4][2] = {
		{-1,0}, {1,0}, {0,-1}, {0,1}
	};

	// Check 4 directions until we find a piece or board edge
	for (int d = 0; d < 4; d++)
	{
		int newFile = file;
		int newRank = rank;

		while (true)
		{
			newFile += directions[d][0];
			newRank += directions[d][1];

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
	}

	return moves;

}