#include "Model.h"
#include <iostream>
#include <filesystem>


// Constructor
Model::Model()
{
    loadModel();
}


bool Model::loadModel() {
    try {
        std::filesystem::path modelPath = std::filesystem::absolute(MODEL_PATH);
        std::cout << "Loading model from: " << modelPath << std::endl;

        std::ifstream input(modelPath, std::ios::binary);
        if (!input) {
            std::cerr << "Failed to open model file stream: " << modelPath << std::endl;
            return false;
        }

        torch::Device device(torch::kCPU);
        m_module = torch::jit::load(input, device);
        m_module.eval();

        std::cout << "Model loaded successfully\n";
        m_modelLoaded = true;
        return true;
    }
    catch (const c10::Error& e) {
        std::cerr << "Torch load failed: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Std exception: " << e.what() << std::endl;
        return false;
    }
}


torch::Tensor Model::boardToTensor(Board& board, GameState& game)
{
    torch::Tensor tensor = torch::zeros({ 18, 8, 8 }, torch::kFloat32);
    bool sideToMoveWhite = game.getCurrentTurn();
    auto getChannel = [&](Board::PieceType piece)
        {
            bool isWhite = piece >= Board::WHITE_PAWN && piece <= Board::WHITE_KING;
            bool myPiece = (isWhite == sideToMoveWhite);
            int base = myPiece ? 0 : 6;

            switch (piece)
            {
            case Board::WHITE_PAWN:
            case Board::BLACK_PAWN:
                return base + 0;

            case Board::WHITE_KNIGHT:
            case Board::BLACK_KNIGHT:
                return base + 1;

            case Board::WHITE_BISHOP:
            case Board::BLACK_BISHOP:
                return base + 2;

            case Board::WHITE_ROOK:
            case Board::BLACK_ROOK:
                return base + 3;

            case Board::WHITE_QUEEN:
            case Board::BLACK_QUEEN:
                return base + 4;

            case Board::WHITE_KING:
            case Board::BLACK_KING:
                return base + 5;

            default:
                return -1;
            }
        };

    for (int file = 0; file < 8; file++)
    {
        for (int rank = 0; rank < 8; rank++)
        {
            Board::PieceType piece = board.getPieceAt(file, rank);

            if (piece == Board::NONE)
                continue;

            int channel = getChannel(piece);

            if (channel >= 0)
            {
                // Python uses tensor[channel,row,col]
                tensor[channel][7 - rank][file] = 1.0f;
            }
        }
    }

    //
    // Castling rights
    //
    if (sideToMoveWhite)
    {
        if (canCastle(board, true, true))
            tensor[12] = 1.0f;

        if (canCastle(board, true, false))
            tensor[13] = 1.0f;

        if (canCastle(board, false, true))
            tensor[14] = 1.0f;

        if (canCastle(board, false, false))
            tensor[15] = 1.0f;
    }
    else
    {
        if (canCastle(board, false, true))
            tensor[12] = 1.0f;

        if (canCastle(board, false, false))
            tensor[13] = 1.0f;

        if (canCastle(board, true, true))
            tensor[14] = 1.0f;

        if (canCastle(board, true, false))
            tensor[15] = 1.0f;
    }

    //
    // En-passant
    //
    if (board.lastDoublePawnMove.file >= 0 && board.lastDoublePawnMove.rank >= 0)
    {
        int epFile = board.lastDoublePawnMove.file;
        int epRank;

        if (sideToMoveWhite)
        {
            // Black just moved two squares
            epRank = (7- board.lastDoublePawnMove.rank) - 1;
        }
        else
        {
            // White just moved two squares
            epRank = (7 - board.lastDoublePawnMove.rank) + 1;
        }

        if (epRank >= 0 && epRank < 8)
        {
            tensor[16][epRank][epFile] = 1.0f;
        }
    }

    //
    // Check indicator
    //
    if (game.getIsCheck())
    {
        tensor[17] = 1.0f;
    }

    return tensor;
}

int Model::promotionOffsetIndex(int fromFile, int toFile)
{
    int delta = toFile - fromFile;

    if (delta == -1)
        return 0; // capture left

    if (delta == 0)
        return 1; // straight

    return 2;     // capture right
}

int Model::moveToPolicyIndex(const Move& move)
{
    // Using 7 - rank as out convention is a1 = [7][0]
    int fromSquare = (7 - move.startRank) * 8 + move.startFile;
    int toSquare = (7 - move.endRank) * 8 + move.endFile;

    //
    // Normal moves + queen promotions
    //
    if (!move.isPromotion || move.promotionPiece == Board::WHITE_QUEEN || move.promotionPiece == Board::BLACK_QUEEN)
    {
        return fromSquare * 64 + toSquare;
    }

    int offset = promotionOffsetIndex(move.startFile, move.endFile);

    //
    // Knight underpromotion
    //
    if (move.promotionPiece == Board::WHITE_KNIGHT ||
        move.promotionPiece == Board::BLACK_KNIGHT)
    {
        return 4096 + fromSquare * 3 + offset;
    }

    //
    // Bishop underpromotion
    //
    if (move.promotionPiece == Board::WHITE_BISHOP ||
        move.promotionPiece == Board::BLACK_BISHOP)
    {
        return 4288 + fromSquare * 3 + offset;
    }

    //
    // Rook underpromotion
    //
    if (move.promotionPiece == Board::WHITE_ROOK ||
        move.promotionPiece == Board::BLACK_ROOK)
    {
        return 4480 + fromSquare * 3 + offset;
    }

    return fromSquare * 64 + toSquare;
}


Move Model::getMove(Board& board, GameState& game)
{
    if (!m_modelLoaded)
    {
        std::cerr << "Model not loaded!" << std::endl;
        return Move{};
    }

    std::vector<Move> legalMoves = game.generateAllLegalMoves(board);
    if (legalMoves.empty())
        return Move{};

    torch::Tensor input = boardToTensor(board, game).unsqueeze(0);
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input);

    // Model returns (policy_logits, value) — must unpack as tuple
    auto output_tuple = m_module.forward(inputs).toTuple();
    at::Tensor policy = output_tuple->elements()[0].toTensor().squeeze(0);  // shape [4672]
    // at::Tensor value = output_tuple->elements()[1].toTensor().squeeze(0); // scalar, if needed

    // Find highest scoring legal move
    float bestScore = -std::numeric_limits<float>::infinity();
    Move bestMove = legalMoves[0];

    for (const auto& move : legalMoves)
    {
        int idx = moveToPolicyIndex(move);
        if (idx >= 0 && idx < static_cast<int>(policy.size(0)))
        {
            float score = policy[idx].item<float>();
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
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