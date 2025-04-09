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

	SDL_DestroySurface(screenSurface);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

}

void SDL_handler::loadPieceTextures()
{
	whitePawn = loadImage("assets/Chess_plt45.svg");
	whiteRook = loadImage("assets/Chess_rlt45.svg");
	whiteKnight = loadImage("assets/Chess_nlt45.svg");
	whiteBishop = loadImage("assets/Chess_blt45.svg");
	whiteQueen = loadImage("assets/Chess_qlt45.svg");
	whiteKing = loadImage("assets/Chess_klt45.svg");

	blackPawn = loadImage("assets/Chess_pdt45.svg");
	blackRook = loadImage("assets/Chess_rdt45.svg");
	blackKnight = loadImage("assets/Chess_ndt45.svg");
	blackBishop = loadImage("assets/Chess_bdt45.svg");
	blackQueen = loadImage("assets/Chess_qdt45.svg");
	blackKing = loadImage("assets/Chess_kdt45.svg");
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
		printf("Could not load image: %s", imageFile);
	}

	SDL_Texture* pieceTexture = SDL_CreateTextureFromSurface(renderer, image);

	return pieceTexture;

}

void SDL_handler::renderPiece(SDL_Texture* piece, int file, int rank)
{

	SDL_FRect dest = { file * squareWidth, rank * squareHeight, squareWidth, squareHeight };

	SDL_RenderTexture(renderer, piece, NULL, &dest);

}

void SDL_handler::render()
{
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);

	renderBoard();

	SDL_RenderPresent(renderer);

}