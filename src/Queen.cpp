#include "Queen.h"

std::vector<Square> Queen::getPossibleMoves(Board& board, int file, int rank)
{
	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));
	const int directions[8][2]{
		{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}
	};

	// Check all 8 directions until not valid
	for (int d = 0; d < 8; d++)
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
				if (targetPiece == Board::NONE)
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