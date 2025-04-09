#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

class SDL_handler
{
private:
	SDL_Texture* whitePawn, * whiteRook, * whiteKnight, * whiteBishop, * whiteQueen, * whiteKing;
	SDL_Texture* blackPawn, * blackRook, * blackKnight, * blackBishop, * blackQueen, * blackKing;

	void loadPieceTextures();

	SDL_Texture* loadImage(std::string imageFile);

public:
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 640;

	const float squareWidth = (float)SCREEN_WIDTH / 8.0f;
	const float squareHeight = (float)SCREEN_HEIGHT / 8.0f;

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

	void renderBoard();

	void renderPiece(SDL_Texture* piece, int file, int rank);

	void render();

};