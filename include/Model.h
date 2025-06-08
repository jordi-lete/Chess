#include "BoardState.h"
#include "GameState.h"
#include <torch/script.h>

#define MODEL_PATH "models/traced_50EPOCH_model.pt"

class Model
{
private:
	torch::jit::script::Module m_module;

public:
	bool loadModel();
	torch::Tensor boardToTensor(Board& board, GameState& game);
	Move getMove(Board& board, GameState& game);
};