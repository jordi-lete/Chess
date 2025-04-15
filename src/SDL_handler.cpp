#include "SDL_handler.h"
#include <iostream>

//Constructor
SDL_handler::SDL_handler()
{
	init();
}

//Destructor
SDL_handler::~SDL_handler() 
{
	cleanup();
}


bool SDL_handler::init()
{

	window = NULL;
	screenSurface = NULL;
	renderer = NULL;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		cleanup();
		return false;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("Powerup Chess", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return false;
		}
		else
		{
			renderer = SDL_CreateRenderer(window, NULL);
		}
	}

	loadPieceTextures();

	return true;

}

void SDL_handler::cleanup()
{
	// Destroy all piece textures
	SDL_DestroyTexture(whitePawn);
	SDL_DestroyTexture(whiteRook);
	SDL_DestroyTexture(whiteKnight);
	SDL_DestroyTexture(whiteBishop);
	SDL_DestroyTexture(whiteQueen);
	SDL_DestroyTexture(whiteKing);

	SDL_DestroyTexture(blackPawn);
	SDL_DestroyTexture(blackRook);
	SDL_DestroyTexture(blackKnight);
	SDL_DestroyTexture(blackBishop);
	SDL_DestroyTexture(blackQueen);
	SDL_DestroyTexture(blackKing);

	SDL_DestroyTexture(possibleMove);

	SDL_DestroySurface(screenSurface);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

}

void SDL_handler::loadPieceTextures()
{
	std::string basePath = SDL_GetBasePath(); // Get the application directory
	whitePawn = loadImage("assets/white_pawn.png");
	whiteRook = loadImage("assets/white_rook.png");
	whiteKnight = loadImage("assets/white_knight.png");
	whiteBishop = loadImage("assets/white_bishop.png");
	whiteQueen = loadImage("assets/white_queen.png");
	whiteKing = loadImage("assets/white_king.png");

	blackPawn = loadImage("assets/black_pawn.png");
	blackRook = loadImage("assets/black_rook.png");
	blackKnight = loadImage("assets/black_knight.png");
	blackBishop = loadImage("assets/black_bishop.png");
	blackQueen = loadImage("assets/black_queen.png");
	blackKing = loadImage("assets/black_king.png");

	possibleMove = loadImage("assets/circle_tp.png");
}

void SDL_handler::renderBoard()
{

	bool white = true; //First square rendered (top left) is white

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			white ? SDL_SetRenderDrawColor(renderer, 234, 250, 215, 255) : SDL_SetRenderDrawColor(renderer, 67, 196, 160, 255);

			SDL_FRect square = { i * squareWidth, j * squareHeight, squareWidth, squareHeight};

			SDL_RenderFillRect(renderer, &square);

			white = !white;

		}
		white = !white;
	}

}

SDL_Texture* SDL_handler::loadImage(std::string imageFile)
{

	SDL_Surface* image = IMG_Load(imageFile.c_str());

	if (!image)
	{
		printf("Could not load image: %s", imageFile.c_str());
	}

	SDL_Texture* pieceTexture = SDL_CreateTextureFromSurface(renderer, image);

	SDL_DestroySurface(image);

	return pieceTexture;

}

void SDL_handler::renderPiece(SDL_Texture* piece, int file, int rank)
{

	SDL_FRect dest = { file * squareWidth, rank * squareHeight, squareWidth, squareHeight };

	SDL_RenderTexture(renderer, piece, NULL, &dest);

}

void SDL_handler::renderPossibleMoves(std::vector<Square> moves)
{

	for (auto& move : moves)
	{
		float xPixel = (move.file + 0.4) * squareWidth;
		float yPixel = (move.rank + 0.4) * squareHeight;
		SDL_FRect dest = { xPixel, yPixel, squareWidth / 5, squareHeight / 5 };
		SDL_RenderTexture(renderer, possibleMove, NULL, &dest);
	}

}

void SDL_handler::render(const Board& board, const GameState& game)
{
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);

	renderBoard();

	if (game.showMoves())
	{
		renderPossibleMoves(game.getMoves());
	}
		
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			Board::PieceType piece = board.getPieceAt(file, rank);
			switch (piece)
			{
			case Board::WHITE_PAWN:
			{
				renderPiece(whitePawn, file, rank);
				break;
			}
			case Board::WHITE_KNIGHT:
			{
				renderPiece(whiteKnight, file, rank);
				break;
			}
			case Board::WHITE_BISHOP:
			{
				renderPiece(whiteBishop, file, rank);
				break;
			}
			case Board::WHITE_ROOK:
			{
				renderPiece(whiteRook, file, rank);
				break;
			}
			case Board::WHITE_QUEEN:
			{
				renderPiece(whiteQueen, file, rank);
				break;
			}
			case Board::WHITE_KING:
			{
				renderPiece(whiteKing, file, rank);
				break;
			}
			case Board::BLACK_PAWN:
			{
				renderPiece(blackPawn, file, rank);
				break;
			}
			case Board::BLACK_KNIGHT:
			{
				renderPiece(blackKnight, file, rank);
				break;
			}
			case Board::BLACK_BISHOP:
			{
				renderPiece(blackBishop, file, rank);
				break;
			}
			case Board::BLACK_ROOK:
			{
				renderPiece(blackRook, file, rank);
				break;
			}
			case Board::BLACK_QUEEN:
			{
				renderPiece(blackQueen, file, rank);
				break;
			}
			case Board::BLACK_KING:
			{
				renderPiece(blackKing, file, rank);
				break;
			}
			}
		}
	}

	SDL_RenderPresent(renderer);
}

Square SDL_handler::snapToBoard(int pixelX, int pixelY)
{
	Square square;

	square.file = pixelX / squareWidth;
	square.rank = pixelY / squareHeight;

	return square;
}