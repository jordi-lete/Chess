#include "Knight.h"

std::vector<Square> Knight::getPossibleMoves(Board& board, int file, int rank)
{

	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));

	const int knightMoves[8][2] = {
		{-2,-1}, {-2,1}, {-1,-2}, {-1,2}, {1,-2}, {1,2}, {2,-1}, {2,1}
	};

	for (int i = 0; i < 8; i++) 
	{
		int newFile = file + knightMoves[i][0];
		int newRank = rank + knightMoves[i][1];

		if (board.isValidPosition(newFile, newRank))
		{
			Board::PieceType targetPiece = board.getPieceAt(newFile, newRank);
			if (targetPiece == board.NONE || board.getPieceColour(targetPiece) != isWhite)
			{
				moves.push_back({ newFile, newRank });
			}
		}
	}
	return moves;

}