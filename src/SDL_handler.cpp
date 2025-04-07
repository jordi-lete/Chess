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
