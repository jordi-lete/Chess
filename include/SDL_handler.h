#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_audio.h>
#include <string>
#include "BoardState.h"
#include "GameState.h"
#include <vector>

struct Sound {
	Uint8* buffer;
	Uint32 length;
	SDL_AudioSpec spec;
};

class SDL_handler
{
private:
	int m_boardSize;
	int m_xOffset = 0;
	int m_yOffset = 0;
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 640;
	float m_squareWidth = (float)SCREEN_WIDTH / 8.0f;
	float m_squareHeight = (float)SCREEN_HEIGHT / 8.0f;

	SDL_Texture* whitePawn, * whiteRook, * whiteKnight, * whiteBishop, * whiteQueen, * whiteKing;
	SDL_Texture* blackPawn, * blackRook, * blackKnight, * blackBishop, * blackQueen, * blackKing;
	SDL_Texture* possibleMove;

	SDL_AudioStream* m_audioStream;
	Sound moveSound;
	Sound captureSound;
	Sound checkSound;

	void cleanup();

	bool init();

	bool initAudio();

	void loadPieceTextures();

	SDL_Texture* loadImage(std::string imageFile);

	bool loadSound(const char* filename, Sound& sound);

	void loadSoundFiles();

	void playSound(const Sound& sound);

	void renderBoard(const GameState& game);

	void renderPiece(SDL_Texture* piece, int file, int rank);

	void renderPossibleMoves(std::vector<Square> moves);

	void renderHeldPiece(const Board& board, int mouseX, int mouseY, Square clickedSquare);

public:

	SDL_Window* window;

	SDL_Renderer* renderer;

	SDL_Surface* screenSurface;

	SDL_Event event;

	//Constructor
	SDL_handler();
	//Destructor
	~SDL_handler();

	void playMoveSound(bool isCapture, bool isCheck);

	void render(const Board& board, const GameState& game, bool holdingPiece = false, int mouseX = 0, int mouseY = 0, Square clickedSquare = {});

	Square snapToBoard(int pixelX, int pixelY);

	void resizeWindow(int width, int height);

	Board::PieceType showPromotionOptions(bool isWhite);

};