#include "SDL_handler.h"

//Constructor
SDL_handler::SDL_handler()
{
	m_boardSize = std::min(SCREEN_WIDTH, SCREEN_HEIGHT);
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

			SDL_FRect square = { (i * m_squareWidth) + m_xOffset, (j * m_squareHeight) + m_yOffset, m_squareWidth, m_squareHeight};

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

void SDL_handler::render(const Board& board, const GameState& game)
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