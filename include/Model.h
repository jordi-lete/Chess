#include "BoardState.h"
#include "GameState.h"
#include <torch/script.h>

#define MODEL_PATH "models/scripted_model.pt"

class Model
{
private:
	torch::jit::script::Module m_module;
	bool m_modelLoaded = false;

public:
	Model(); //Constructor
	bool loadModel();
	torch::Tensor boardToTensor(Board& board, GameState& game);
	int promotionOffsetIndex(int fromFile, int toFile);
	int moveToPolicyIndex(const Move& move);
	Move getMove(Board& board, GameState& game);
	bool makeAIMove(Board& board, GameState& game);
	bool canCastle(Board& board, bool isWhite, bool kingSide);
};