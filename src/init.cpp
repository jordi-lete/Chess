#include "init.h"
#include "SDL_handler.h"
#include <iostream>
#include "BoardState.h"

void Chess::run()
{
	Board board;

	SDL_handler handler;

	handler.render(board);

	bool running = true;
	int xStart = -1;
	int yStart = -1;
	int xEnd = -1;
	int yEnd = -1;
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
			clickedPiece = board.getPieceAt(clickedSquare.file, clickedSquare.rank);
			std::cout << "Piece clicked was: " << clickedPiece << std::endl;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			xEnd = handler.event.button.x;
			yEnd = handler.event.button.y;
			placeSquare = handler.snapToBoard(xEnd, yEnd);
			board.squares[placeSquare.file][placeSquare.rank] = clickedPiece;
			board.squares[clickedSquare.file][clickedSquare.rank] = board.NONE;
			break;
		}

		}

		handler.render(board);
	}

}
