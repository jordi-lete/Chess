#include "RunGame.h"
#include "SDL_handler.h"
#include "BoardState.h"
#include "GameState.h"
#include <iostream>

void Chess::run()
{
	Board board;
	GameState game;
	SDL_handler handler;

	bool running = true;
	Square clickedSquare;
	Square placeSquare;
	Square lastClick;
	bool useLastClick = false;

	/* -------- Render piece while dragging it -------- */
	bool holdingPiece = false;
	int mouseX = 0;
	int mouseY = 0;
	/* ------------------------------------------------ */

	handler.render(board, game);

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
		case SDL_EVENT_WINDOW_RESIZED:
		{
			int width = handler.event.window.data1;
			int height = handler.event.window.data2;
			handler.resizeWindow(width, height);
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			int xStart = handler.event.button.x;
			int yStart = handler.event.button.y;
			mouseX = xStart;
			mouseY = yStart;
			clickedSquare = handler.snapToBoard(xStart, yStart);
			Board::PieceType piece = board.getPieceAt(clickedSquare.file, clickedSquare.rank);
			if (piece != board.NONE && board.getPieceColour(piece) == game.getCurrentTurn())
			{
				holdingPiece = true;
			}
			game.getPossibleMoves(board, clickedSquare.file, clickedSquare.rank);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			holdingPiece = false;
			int xEnd = handler.event.button.x;
			int yEnd = handler.event.button.y;
			placeSquare = handler.snapToBoard(xEnd, yEnd);
			// Check if it is a click (rather than a drag)
			if (clickedSquare.file == placeSquare.file && clickedSquare.rank == placeSquare.rank)
			{
				Board::PieceType clickedPiece = board.getPieceAt(clickedSquare.file, clickedSquare.rank);
				bool isPlayerPiece = (clickedPiece != Board::NONE && board.getPieceColour(clickedPiece) == game.getCurrentTurn());
				if (isPlayerPiece)
				{
					lastClick = clickedSquare;
					useLastClick = true;
					game.getPossibleMoves(board, clickedSquare.file, clickedSquare.rank);
				}
				else if (useLastClick)
				{
					game.tryMakeMove(board, lastClick.file, lastClick.rank, placeSquare.file, placeSquare.rank);
					useLastClick = false;
				}
			}
			// Otherwise it's a drag move
			else
			{
				game.tryMakeMove(board, clickedSquare.file, clickedSquare.rank, placeSquare.file, placeSquare.rank);
				useLastClick = false;
			}
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		{
			if (holdingPiece)
			{
				mouseX = handler.event.motion.x;
				mouseY = handler.event.motion.y;
			}
			break;
		}
		case SDL_EVENT_KEY_DOWN:
		{
			if (handler.event.key.scancode == SDL_SCANCODE_LEFT)
			{
				game.undoLastMove(board);
			}
			break;
		}
		}

		if (game.promotionInProgress)
		{
			bool isWhite = game.getCurrentTurn();
			Board::PieceType promoteTo = handler.showPromotionOptions(isWhite);
			game.completePromotion(board, promoteTo);
		}

		handler.render(board, game, holdingPiece, mouseX, mouseY, clickedSquare);
		if (game.moveMade)
		{
			handler.playMoveSound(game.getIsCapture(), game.getIsCheck());
			game.moveMade = false;
		}

		// check for game end
		if (game.gameOver)
		{
			if (game.getCurrentTurn())
			{
				std::cout << "Black wins!" << std::endl;
			}
			else
			{
				std::cout << "White wins!" << std::endl;
			}
			// Switch back to false so that the message doesn't keep printing
			game.gameOver = false;
		}
	}

}
