#include "Evaluate.h"

int Evaluate::evaluatePosition(Board& board)
{
	int whiteMaterial = countMaterial(board, true);
	int blackMaterial = countMaterial(board, false);

	int evaluation = whiteMaterial - blackMaterial;

	return evaluation;
}

int Evaluate::countMaterial(Board& board, bool isWhite)
{
	int material = 0;

	for (int f = 0; f < 8; f++)
	{
		for (int r = 0; r < 8; r++)
		{
			Board::PieceType piece = board.getPieceAt(f, r);
			if (piece != board.NONE && board.getPieceColour(piece) == isWhite)
			{
				material += m_pieceValue.at(piece);
			}
		}
	}

	return material;
}