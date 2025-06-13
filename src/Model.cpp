#include "Model.h"
#include <iostream>


// Constructor
Model::Model()
{
    loadModel();
}


bool Model::loadModel() {
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load()
        torch::Device cpu_device(torch::kCPU);
        m_module = torch::jit::load(MODEL_PATH, cpu_device);
        //m_module = torch::jit::load(MODEL_PATH);
        m_module.eval(); // Set to evaluation mode
        std::cout << "Model loaded successfully\n";
        m_modelLoaded = true;
        return true;
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.msg() << "\n";
        m_modelLoaded = false;
        return false;
    }
}


torch::Tensor Model::boardToTensor(Board& board, GameState& game) {
    torch::Tensor tensor = torch::zeros({ 19, 8, 8 }, torch::kFloat32);

    for (int file = 0; file < 8; file++)
    {
        for (int rank = 0; rank < 8; rank++)
        {
            Board::PieceType piece = board.getPieceAt(file, rank);
            if (piece != Board::NONE)
            {
                tensor[piece - 1][7 - rank][file] = 1.0; // Flip board rank as our conversion is flipped (e.g. a1 = [7][0])
            }
        }
    }

    // Castling rights
    if (canCastle(board, true, true)) // White kingside castle
    {
        tensor[12] = torch::ones({ 8, 8 }, torch::kFloat32);
    }
    if (canCastle(board, true, false)) // White queenside castle
    {
        tensor[13] = torch::ones({ 8, 8 }, torch::kFloat32);
    }
    if (canCastle(board, false, true)) // Black kingside castle
    {
        tensor[14] = torch::ones({ 8, 8 }, torch::kFloat32);
    }
    if (canCastle(board, false, false)) // Black queenside castle
    {
        tensor[15] = torch::ones({ 8, 8 }, torch::kFloat32);
    }
    
    // En passant
    if (board.lastDoublePawnMove.file >= 0 && board.lastDoublePawnMove.rank >= 0) {
        // The en passant target square is behind the pawn that just moved
        int epTargetRank = board.lastDoublePawnMove.rank + (game.getCurrentTurn() ? 1 : -1);
        if (epTargetRank >= 0 && epTargetRank < 8) {
            tensor[16][epTargetRank][board.lastDoublePawnMove.file] = 1.0f;
        }
    }

    // Current turn (white = 1, black = 0)
    if (game.getCurrentTurn()) {
        tensor[17] = torch::ones({ 8, 8 }, torch::kFloat32);
    }

    // Check
    if (game.getIsCheck())
    {
        tensor[18] = torch::ones({ 8, 8 }, torch::kFloat32);
    }

    return tensor;
}


int Model::moveToPolicyIndex(const Move& move) {
    // Using 7 - rank as out convention is a1 = [7][0]
    int from_square = (7 - move.startRank) * 8 + move.startFile;
    int to_square = (7 - move.endRank) * 8 + move.endFile;
    std::cout << "from square: " << from_square << std::endl;
    std::cout << "to square: " << to_square << std::endl;

    int base_index = from_square * 64 + to_square;
    std::cout << "base index: " << base_index << std::endl;

    // Handle promotions
    if (move.isPromotion) {
        if (move.promotionPiece == Board::WHITE_QUEEN || move.promotionPiece == Board::BLACK_QUEEN) {
            return base_index; // Queen promotion uses base index
        }
        else if (move.promotionPiece == Board::WHITE_KNIGHT || move.promotionPiece == Board::BLACK_KNIGHT) {
            return 4096 + from_square; // Indices 4096-4159
        }
        else if (move.promotionPiece == Board::WHITE_BISHOP || move.promotionPiece == Board::BLACK_BISHOP) {
            return 4160 + from_square; // Indices 4160-4223
        }
        else if (move.promotionPiece == Board::WHITE_ROOK || move.promotionPiece == Board::BLACK_ROOK) {
            return 4224 + from_square; // Indices 4224-4287
        }
    }

    return base_index;
}


Move Model::getMove(Board& board, GameState& game)
{
    if (!m_modelLoaded)
    {
        std::cerr << "Model not loaded!" << std::endl;
        return Move{};
    }

    std::vector<Move> legalMoves = game.generateAllLegalMoves(board);
    if (legalMoves.empty()) {
        return Move{}; // No legal moves available
    }

    torch::Tensor input = boardToTensor(board, game);
    input = input.unsqueeze(0); // Add batch dimension (1, 19, 8, 8)

    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input);

    at::Tensor output = m_module.forward(inputs).toTensor();
    output = output.squeeze(0); // Remove batch dimension

    // Find the highest scoring legal move
    float bestScore = -std::numeric_limits<float>::infinity();
    Move bestMove;
    bool foundValidMove = false;

    for (const auto& move : legalMoves) {
        int moveIndex = moveToPolicyIndex(move);

        if (moveIndex < output.size(0)) {
            float score = output[moveIndex].item<float>();
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
                foundValidMove = true;
            }
        }
    }

    if (!foundValidMove && !legalMoves.empty()) {
        // Fallback to random legal move if no valid model output
        bestMove = legalMoves[0];
        std::cout << "Warning: Using fallback random move selection" << std::endl;
    }

    return bestMove;
}


bool Model::makeAIMove(Board& board, GameState& game)
{
    if (!m_modelLoaded)
    {
        std::cerr << "Model not loaded!" << std::endl;
        return false;
    }
    Move bestMove = getMove(board, game);

    return game.tryMakeMove(board, bestMove.startFile, bestMove.startRank, bestMove.endFile, bestMove.endRank);
}


bool Model::canCastle(Board& board, bool isWhite, bool kingSide)
{

    if (isWhite && board.whiteKingMoved || !isWhite && board.blackKingMoved)
    {
        return false;
    }

    int rank = isWhite ? 7 : 0;

    if (kingSide)
    {
        if (isWhite && board.whiteRookKSMoved || !isWhite && board.blackRookKSMoved)
        {
            return false;
        }
    }

    else // Queenside
    {
        if (isWhite && board.whiteRookQSMoved || !isWhite && board.blackRookQSMoved)
        {
            return false;
        }
    }
    return true;

}