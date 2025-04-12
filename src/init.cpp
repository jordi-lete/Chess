#include "init.h"
#include "SDL_handler.h"
#include <iostream>
#include "BoardState.h"
#include "GameState.h"

void Chess::run()
{
	Board board;
	GameState game;
	SDL_handler handler;

	handler.render(board);

	bool running = true;
	bool isWhiteTurn = true;
	bool isWhitePiece = true;
	int xStart = -1;
	int yStart = -1;
	Board::PieceType clickedPiece = board.NONE;
	Square clickedSquare;
	Square placeSquare;

	// start an event loop
	while (running && SDL_WaitEvent(&handler.event)) 
	{
		//std::cout << "Event: " << handler.event.type << std::endl;
		switch (handler.event.type)
		{
		case SDL_EVENT_QUIT:
		{
			running = false;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			xStart = handler.event.button.x;
			yStart = handler.event.button.y;
			clickedSquare = handler.snapToBoard(xStart, yStart);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			int xEnd = handler.event.button.x;
			int yEnd = handler.event.button.y;
			placeSquare = handler.snapToBoard(xEnd, yEnd);
			game.tryMakeMove(board, clickedSquare.file, clickedSquare.rank, placeSquare.file, placeSquare.rank);
			break;
		}

		}

		handler.render(board);
	}

}
