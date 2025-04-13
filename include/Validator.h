#pragma once
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"
#include "BoardState.h"
#include <memory>

class Validator
{
private:
	std::unique_ptr<Piece> pawnValidator;
	std::unique_ptr<Piece> knightValidator;
	std::unique_ptr<Piece> bishopValidator;
	std::unique_ptr<Piece> rookValidator;
	std::unique_ptr<Piece> queenValidator;
	std::unique_ptr<Piece> kingValidator;

public:
	// Constructor
	Validator()
	{
		pawnValidator = std::make_unique<Pawn>();
		knightValidator = std::make_unique<Knight>();
		bishopValidator = std::make_unique<Bishop>();
		rookValidator = std::make_unique<Rook>();
		queenValidator = std::make_unique<Queen>();
		kingValidator = std::make_unique<King>();
	}

	Piece* getValidator(Board::PieceType piece)
	{
		switch (piece)
		{
		case Board::WHITE_PAWN:
		case Board::BLACK_PAWN:
		{
			return pawnValidator.get();
		}
		case Board::WHITE_KNIGHT:
		case Board::BLACK_KNIGHT:
		{
			return knightValidator.get();
		}
		case Board::WHITE_BISHOP:
		case Board::BLACK_BISHOP:
		{
			return bishopValidator.get();
		}
		case Board::WHITE_ROOK:
		case Board::BLACK_ROOK:
		{
			return rookValidator.get();
		}
		case Board::WHITE_QUEEN:
		case Board::BLACK_QUEEN:
		{
			return queenValidator.get();
		}
		case Board::WHITE_KING:
		case Board::BLACK_KING:
		{
			return kingValidator.get();
		}
		default:
			return nullptr;
		}

	}

};