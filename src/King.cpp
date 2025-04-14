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

	// ADD CASTLES

	return moves;

}