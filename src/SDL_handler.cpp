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

	return true;

}

void SDL_handler::cleanup()
{

	SDL_DestroySurface(screenSurface);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

}

void SDL_handler::renderBoard()
{

	bool white = true; //First square rendered (top left) is white

	float squareWidth = (float)SCREEN_WIDTH / 8.0f;
	float squareHeight = (float)SCREEN_HEIGHT / 8.0f;

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

void SDL_handler::render()
{
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);

	renderBoard();

	SDL_RenderPresent(renderer);

}