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

	checkForEnPassant(board, file, rank, isWhite, moves);

	return moves;

}

void Pawn::checkForEnPassant(Board& board, int file, int rank, bool isWhite, std::vector<Square>& moves)
{
	// if the last move was a double pawn move
	if (board.lastDoublePawnMove.file >= 0)
	{
		int enPassantRank = isWhite ? 3 : 4;
		// if the current pawn is on the capture rank and the target pawn is in the file next to it
		if (rank == enPassantRank && std::abs(file - board.lastDoublePawnMove.file) == 1)
		{
			int captureDirection = isWhite ? -1 : 1;
			// Update moves inplace
			moves.push_back({board.lastDoublePawnMove.file, rank + captureDirection});
		}
	}
}