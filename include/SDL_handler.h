#pragma once
#include <SDL3/SDL.h>

class SDL_handler
{
public:
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 640;

	SDL_Window* window;

	SDL_Renderer* renderer;

	SDL_Surface* screenSurface;

	SDL_Event event;

	//Constructor
	SDL_handler();
	//Destructor
	~SDL_handler();

	void cleanup();

	bool init();

};