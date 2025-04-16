#include "init.h"
#include "SDL_handler.h"
#include "BoardState.h"
#include "GameState.h"

void Chess::run()
{
	Board board;
	GameState game;
	SDL_handler handler;

	handler.render(board, game);

	bool running = true;
	Square clickedSquare;
	Square placeSquare;
	Square lastClick;
	bool useLastClick = false;

	// start an event loop
	while (running && SDL_WaitEvent(&handler.event)) 
	{
		switch (handler.event.type)
		{
		case SDL_EVENT_QUIT:
		{
			running = false;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			int xStart = handler.event.button.x;
			int yStart = handler.event.button.y;
			clickedSquare = handler.snapToBoard(xStart, yStart);
			game.getPossibleMoves(board, clickedSquare.file, clickedSquare.rank);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			int xEnd = handler.event.button.x;
			int yEnd = handler.event.button.y;
			placeSquare = handler.snapToBoard(xEnd, yEnd);
			if (clickedSquare.file == placeSquare.file && clickedSquare.rank == placeSquare.rank)
			{
				if (!useLastClick)
				{
					useLastClick = true;
					lastClick = clickedSquare;
				}
				else
				{
					game.tryMakeMove(board, lastClick.file, lastClick.rank, placeSquare.file, placeSquare.rank);
					useLastClick = false;
				}
			}
			else
			{
				game.tryMakeMove(board, clickedSquare.file, clickedSquare.rank, placeSquare.file, placeSquare.rank);
				useLastClick = false;
			}
			break;
		}
		}

		handler.render(board, game);
	}

}
