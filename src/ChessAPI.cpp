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
        int try_move(int fromFile, int fromRank, int toFile, int toRank) {
        return game.tryMakeMove(board, fromFile, fromRank, toFile, toRank);
    }

    EMSCRIPTEN_KEEPALIVE
        int get_piece(int file, int rank) {
        return board.getPieceAt(file, rank);
    }

    EMSCRIPTEN_KEEPALIVE
        int get_current_turn() {
        return game.getCurrentTurn(); // 0 for white, 1 for black
    }

    EMSCRIPTEN_KEEPALIVE
        bool is_game_over() {
        return game.gameOver;
    }

}