#include "Model.h"
#include <iostream>


bool Model::loadModel() {
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        m_module = torch::jit::load(MODEL_PATH);
        m_module.eval(); // Set to evaluation mode
        std::cout << "Model loaded successfully\n";
        return true;
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.msg() << "\n";
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
                tensor[piece - 1][rank][file] = 1.0;
            }
        }
    }

    // Castling rights
    
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


Move Model::getMove(Board& board, GameState& game)
{
    torch::Tensor input = boardToTensor(board, game);
    input = input.unsqueeze(0); // Add batch dimension (1, 19, 8, 8)

    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input);

    at::Tensor output = m_module.forward(inputs).toTensor();
    output = output.squeeze(0); // Remove batch dimension
    auto max_result = torch::max(output, 0);
    int max_index = std::get<1>(max_result).item<int>();

    Move modelMove;
    int from_square = max_index / 64;
    int to_square = max_index % 64;

    // Convert square indices to file/rank coordinates
    int from_file = from_square % 8;
    int from_rank = from_square / 8;
    int to_file = to_square % 8;
    int to_rank = to_square / 8;

    // Create the move
    modelMove.startFile = from_file;
    modelMove.startRank = from_rank;
    modelMove.endFile = to_file;
    modelMove.endRank = to_rank;
    modelMove.movingPiece = board.getPieceAt(from_file, from_rank);

    // Check for castling
    if ((modelMove.movingPiece == Board::WHITE_KING || modelMove.movingPiece == Board::BLACK_KING) &&
        std::abs(to_file - from_file) == 2)
    {
        modelMove.isCastling = true;
    }

    // Check for en passant
    if ((modelMove.movingPiece == Board::WHITE_PAWN || modelMove.movingPiece == Board::BLACK_PAWN) &&
        to_file != from_file && board.getPieceAt(to_file, to_rank) == Board::NONE)
    {
        modelMove.isEnPassant = true;
    }

    // Check for promotion
    bool isWhite = board.getPieceColour(modelMove.movingPiece);
    int promotionRank = isWhite ? 0 : 7;
    if ((modelMove.movingPiece == Board::WHITE_PAWN || modelMove.movingPiece == Board::BLACK_PAWN) && to_rank == promotionRank)
    {
        modelMove.isPromotion = true;
        // TODO: Default queen promotion - can we get other promotion pieces from the engine?
        modelMove.promotionPiece = isWhite ? Board::WHITE_QUEEN : Board::BLACK_QUEEN;
    }

    return modelMove;
}
