#include "SDL_handler.h"

//Constructor
SDL_handler::SDL_handler()
{
	m_boardSize = std::min(SCREEN_WIDTH, SCREEN_HEIGHT);
	m_audioStream = nullptr;
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

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		cleanup();
		return false;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("Powerup Chess", m_boardSize, m_boardSize, SDL_WINDOW_RESIZABLE);
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
	if (!initAudio())
	{
		printf("Failed to initialise Audio\n");
	}
	loadSoundFiles();

	return true;

}

bool SDL_handler::initAudio()
{
	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = SDL_AUDIO_F32;
	desired.channels = 2;

	m_audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, NULL, NULL);
	if (!m_audioStream)
	{
		printf("Failed to open audio device: %s\n", SDL_GetError());
		return false;
	}
	SDL_ResumeAudioStreamDevice(m_audioStream);
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

	// Free WAV buffers
	SDL_free(moveSound.buffer);
	SDL_free(captureSound.buffer);
	SDL_free(checkSound.buffer);
	// Close audio device
	SDL_DestroyAudioStream(m_audioStream);

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

void SDL_handler::loadSoundFiles()
{
	loadSound("sounds/move-self.wav", moveSound);
	loadSound("sounds/capture.wav", captureSound);
	loadSound("sounds/move-check.wav", checkSound);
}


bool SDL_handler::loadSound(const char* filename, Sound& sound)
{
	SDL_AudioSpec wavSpec;
	Uint8* wavBuffer;
	Uint32 wavLength;

	if (SDL_LoadWAV(filename, &wavSpec, &wavBuffer, &wavLength) == NULL) {
		printf("Failed to load WAV: %s\n", SDL_GetError());
		return false;
	}

	sound.buffer = wavBuffer;
	sound.length = wavLength;
	sound.spec = wavSpec;

	return true;
}

void SDL_handler::playSound(const Sound& sound)
{
	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = SDL_AUDIO_F32;
	desired.channels = 2;

	SDL_AudioStream* tempStream = SDL_CreateAudioStream(&sound.spec, &desired);
	if (!tempStream)
	{
		printf("Failed to create audio stream %s\n", SDL_GetError());
		return;
	}

	if (SDL_PutAudioStreamData(tempStream, sound.buffer, sound.length) == -1)
	{
		printf("Failed to put data into stream %s\n", SDL_GetError());
		return;
	}

	const int bufferSize = 4096;
	float buffer[bufferSize];

	int bytesRead;
	while ((bytesRead = SDL_GetAudioStreamData(tempStream, buffer, bufferSize)) > 0)
	{
		if (SDL_PutAudioStreamData(m_audioStream, buffer, bytesRead) < 0)
		{
			printf("Failed to put data into master stream: %s\n", SDL_GetError());
			break;
		}
	}

	SDL_DestroyAudioStream(tempStream);
}

void SDL_handler::playMoveSound(bool isCapture, bool isCheck)
{
	if (isCapture)
	{
		playSound(captureSound);
	}
	else if (isCheck)
	{
		playSound(checkSound);
	}
	else
	{
		playSound(moveSound);
	}
}

void SDL_handler::renderBoard()
{

	bool white = true; //First square rendered (top left) is white

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			white ? SDL_SetRenderDrawColor(renderer, 234, 250, 215, 255) : SDL_SetRenderDrawColor(renderer, 67, 196, 160, 255);

			SDL_FRect square = { (i * m_squareWidth) + m_xOffset, (j * m_squareHeight) + m_yOffset, m_squareWidth, m_squareHeight };

			SDL_RenderFillRect(renderer, &square);

			white = !white;

		}
		white = !white;
	}

}

void SDL_handler::renderPiece(SDL_Texture* piece, int file, int rank)
{

	SDL_FRect dest = { (file * m_squareWidth) + m_xOffset, (rank * m_squareHeight) + m_yOffset, m_squareWidth, m_squareHeight };

	SDL_RenderTexture(renderer, piece, NULL, &dest);

}

void SDL_handler::renderPossibleMoves(std::vector<Square> moves)
{

	for (auto& move : moves)
	{
		float xPixel = (move.file + 0.4) * m_squareWidth;
		float yPixel = (move.rank + 0.4) * m_squareHeight;
		SDL_FRect dest = { xPixel + m_xOffset, yPixel + m_yOffset, m_squareWidth / 5, m_squareHeight / 5 };
		SDL_RenderTexture(renderer, possibleMove, NULL, &dest);
	}

}

void SDL_handler::renderHeldPiece(const Board& board, int mouseX, int mouseY, Square clickedSquare)
{
	/* -------- Render square tile -------- */
	bool isLightSquare = (clickedSquare.file + clickedSquare.rank) % 2 == 0;
	isLightSquare ? SDL_SetRenderDrawColor(renderer, 234, 250, 215, 255) : SDL_SetRenderDrawColor(renderer, 67, 196, 160, 255);
	SDL_FRect square = { (clickedSquare.file * m_squareWidth) + m_xOffset, (clickedSquare.rank * m_squareHeight) + m_yOffset, m_squareWidth, m_squareHeight };
	SDL_RenderFillRect(renderer, &square);
	/* ------------------------------------ */

	/*  Now render piece in mouse position  */
	Board::PieceType piece = board.getPieceAt(clickedSquare.file, clickedSquare.rank);
	SDL_Texture* pieceTexture = nullptr;
	switch (piece)
	{
	case Board::WHITE_PAWN: pieceTexture = whitePawn; break;
	case Board::WHITE_KNIGHT: pieceTexture = whiteKnight; break;
	case Board::WHITE_BISHOP: pieceTexture = whiteBishop; break;
	case Board::WHITE_ROOK: pieceTexture = whiteRook; break;
	case Board::WHITE_QUEEN: pieceTexture = whiteQueen; break;
	case Board::WHITE_KING: pieceTexture = whiteKing; break;
	case Board::BLACK_PAWN: pieceTexture = blackPawn; break;
	case Board::BLACK_KNIGHT: pieceTexture = blackKnight; break;
	case Board::BLACK_BISHOP: pieceTexture = blackBishop; break;
	case Board::BLACK_ROOK: pieceTexture = blackRook; break;
	case Board::BLACK_QUEEN: pieceTexture = blackQueen; break;
	case Board::BLACK_KING: pieceTexture = blackKing; break;
	}
	SDL_FRect dest = { mouseX - (m_squareWidth / 2), mouseY - (m_squareHeight / 2), m_squareWidth, m_squareHeight};
	SDL_RenderTexture(renderer, pieceTexture, NULL, &dest);
	/* ----------------------------------- */

}

void SDL_handler::render(const Board& board, const GameState& game, bool holdingPiece, int mouseX, int mouseY, Square clickedSquare)
{
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);

	renderBoard();
		
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

	if (game.showMoves())
	{
		renderPossibleMoves(game.getMoves());
	}

	if (holdingPiece)
	{
		renderHeldPiece(board, mouseX, mouseY, clickedSquare);
	}

	SDL_RenderPresent(renderer);
}

Square SDL_handler::snapToBoard(int pixelX, int pixelY)
{
	Square square;

	square.file = (pixelX - m_xOffset) / m_squareWidth;
	square.rank = (pixelY - m_yOffset) / m_squareHeight;

	return square;
}

void SDL_handler::resizeWindow(int width, int height)
{
	m_boardSize = std::min(width, height);
	m_squareWidth = (float)m_boardSize / 8.0f;
	m_squareHeight = (float)m_boardSize / 8.0f;
	if (width > height)
	{
		m_xOffset = (width - height) / 2;
		m_yOffset = 0;
	}
	else if (height > width)
	{
		m_xOffset = 0;
		m_yOffset = (height - width) / 2;
	}
	else
	{
		m_xOffset = 0;
		m_yOffset = 0;
	}
}

Board::PieceType SDL_handler::showPromotionOptions(bool isWhite)
{

	bool waitingForSelection = true;
	SDL_Event promoteEvent;

	// Dim the background
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_FRect fullscreen = { m_xOffset, m_yOffset, (float)m_boardSize, (float)m_boardSize };
	SDL_RenderFillRect(renderer, &fullscreen);

	// Calculate the promotion box dimensions and position
	float padding = 1.1;
	float boxWidth = (m_squareWidth * 4) * padding;
	float boxHeight = m_squareHeight * padding;
	float boxX = m_xOffset + m_boardSize / 2 - boxWidth / 2; // Center horizontally
	float boxY = m_yOffset + m_boardSize / 2 - boxHeight / 2; // Center vertically

	// Draw selection box
	SDL_SetRenderDrawColor(renderer, 57, 158, 118, 200);
	SDL_FRect selectionBox = { boxX, boxY, boxWidth, boxHeight };
	SDL_RenderFillRect(renderer, &selectionBox);

	SDL_Texture* promotionPieces[4];
	Board::PieceType pieceType[4];
	if (isWhite)
	{
		promotionPieces[0] = whiteKnight;
		promotionPieces[1] = whiteBishop;
		promotionPieces[2] = whiteRook;
		promotionPieces[3] = whiteQueen;

		pieceType[0] = Board::WHITE_KNIGHT;
		pieceType[1] = Board::WHITE_BISHOP;
		pieceType[2] = Board::WHITE_ROOK;
		pieceType[3] = Board::WHITE_QUEEN;
	}
	else
	{
		promotionPieces[0] = blackKnight;
		promotionPieces[1] = blackBishop;
		promotionPieces[2] = blackRook;
		promotionPieces[3] = blackQueen;

		pieceType[0] = Board::BLACK_KNIGHT;
		pieceType[1] = Board::BLACK_BISHOP;
		pieceType[2] = Board::BLACK_ROOK;
		pieceType[3] = Board::BLACK_QUEEN;
	}

	// Render the promotion pieces
	for (int i = 0; i < 4; i++) {
		SDL_FRect pieceRect = {
			boxX + m_squareWidth * ((padding - 1) / 2) + i * (boxWidth / 4),
			boxY + m_squareHeight * ((padding - 1) / 2),
			m_squareWidth,
			m_squareHeight
		};
		SDL_RenderTexture(renderer, promotionPieces[i], NULL, &pieceRect);
	}

	SDL_RenderPresent(renderer);

	Board::PieceType selection = Board::NONE;
	int squareX = -1;
	bool insideY = false;

	while (waitingForSelection && SDL_WaitEvent(&promoteEvent))
	{
		if (promoteEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			int xStart = promoteEvent.button.x;
			int yStart = promoteEvent.button.y;
			squareX = floor((xStart - boxX) / (boxWidth / 4));
			insideY = floor((yStart - boxY) / (boxHeight)) == 0;
		}
		else if (promoteEvent.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			int xEnd = promoteEvent.button.x;
			int yEnd = promoteEvent.button.y;
			int squareXEnd = floor((xEnd - boxX) / (boxWidth / 4));
			bool insideYEnd = floor((yEnd - boxY) / (boxHeight)) == 0;
			// If we click inside box and the relase position is the same square as click position
			if (insideY && insideYEnd && squareX > -1 && squareX < 4 && squareX == squareXEnd)
			{
				selection = pieceType[squareX];
			}
			waitingForSelection = false;
		}
		else if (promoteEvent.type == SDL_EVENT_QUIT)
		{
			waitingForSelection = false;
		}
		else if (promoteEvent.type == SDL_EVENT_WINDOW_RESIZED)
		{
			int width = promoteEvent.window.data1;
			int height = promoteEvent.window.data2;
			resizeWindow(width, height);
		}
	}

	return selection;
}