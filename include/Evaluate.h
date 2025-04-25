#pragma once
#include "BoardState.h"
#include <unordered_map>

namespace Evaluate
{

	static const int m_pawnValue = 100;
	static const int m_knightValue = 300;
	static const int m_bishopValue = 300;
	static const int m_rookValue = 500;
	static const int m_queenValue = 900;

	static const std::unordered_map<Board::PieceType, int> m_pieceValue = 
	{
		{Board::WHITE_PAWN, m_pawnValue}, {Board::BLACK_PAWN, m_pawnValue},
		{Board::WHITE_KNIGHT, m_knightValue}, {Board::BLACK_KNIGHT, m_knightValue},
		{Board::WHITE_BISHOP, m_bishopValue}, {Board::BLACK_BISHOP, m_bishopValue},
		{Board::WHITE_ROOK, m_rookValue}, {Board::BLACK_ROOK, m_rookValue},
		{Board::WHITE_QUEEN, m_queenValue}, {Board::BLACK_QUEEN, m_queenValue},
		{Board::WHITE_KING, 0}, {Board::BLACK_KING, 0}
	};

	int evaluatePosition(Board& board);

	int countMaterial(Board& board, bool isWhite);

};