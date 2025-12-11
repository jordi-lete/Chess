#include "BoardState.h"
#include "GameState.h"
#include <emscripten.h>

static Board board;
static GameState game;

extern "C" {

    EMSCRIPTEN_KEEPALIVE
        void init_game() {
        board = Board();
        game = GameState();
    }

    EMSCRIPTEN_KEEPALIVE
        int try_move(int fromFile, int fromRank, int toFile, int toRank, int promotionPiece = 0) {
        return game.tryMakeMove(board, fromFile, fromRank, toFile, toRank, static_cast<Board::PieceType>(promotionPiece));
    }

    EMSCRIPTEN_KEEPALIVE
        int get_piece(int file, int rank) {
        return board.getPieceAt(file, rank);
    }

    EMSCRIPTEN_KEEPALIVE
        int get_current_turn() {
        return game.getCurrentTurn(); // 0 for black, 1 for white
    }

    EMSCRIPTEN_KEEPALIVE
        bool is_game_over() {
        return game.gameOver;
    }

    EMSCRIPTEN_KEEPALIVE
    int get_legal_moves(int file, int rank, int* legalMoves) {
        game.getPossibleMoves(board, file, rank);
        const std::vector<Square>& moves = game.getMoves();

        for (int i = 0; i < moves.size(); i++) {
            legalMoves[i * 2] = moves[i].file;
            legalMoves[i * 2 + 1] = moves[i].rank;
        }

        return moves.size(); // number of moves
    }

}