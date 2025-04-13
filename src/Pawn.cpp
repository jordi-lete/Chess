#include "Pawn.h"

std::vector<Square> Pawn::getPossibleMoves(Board& board, int file, int rank)
{

	std::vector<Square> moves;
	bool isWhite = board.getPieceColour(board.getPieceAt(file, rank));
	int direction = isWhite ? -1 : 1;

	// First get standard push move
	if (board.isValidPosition(file, rank + direction) && board.getPieceAt(file, rank + direction) == Board::NONE)
	{
		moves.push_back({ file, rank + direction });
		// Check if pawn is on it's start square
		if ((isWhite && rank == 6) || (!isWhite && rank == 1))
		{
			if (board.getPieceAt(file, rank + 2 * direction) == Board::NONE)
			{
				moves.push_back({ file, rank + 2 * direction });
			}
		}
	}

	// Now check for captures
	if (board.isValidPosition(file - 1, rank + direction))
	{
		Board::PieceType targetPiece = board.getPieceAt(file - 1, rank + direction);
		if (targetPiece != Board::NONE && board.getPieceColour(targetPiece) != isWhite)
		{
			moves.push_back({ file - 1, rank + direction });
		}
	}
	if (board.isValidPosition(file + 1, rank + direction))
	{
		Board::PieceType targetPiece = board.getPieceAt(file + 1, rank + direction);
		if (targetPiece != Board::NONE && board.getPieceColour(targetPiece) != isWhite)
		{
			moves.push_back({ file + 1, rank + direction });
		}
	}

	// EN-PASSANT

	return moves;

}