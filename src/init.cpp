#include "init.h"
#include "SDL_handler.h"
#include <iostream>

void Chess::run()
{

	SDL_handler handler;

	bool running = true;


		// start an event loop
	while (running && SDL_WaitEvent(&handler.event)) 
	{
		std::cout << "Event: " << handler.event.type << std::endl;
		switch (handler.event.type)
		{
		case SDL_EVENT_QUIT:
		{
			running = false;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			
		}
		}
	}


}
