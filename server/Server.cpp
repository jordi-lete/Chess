#include "BoardState.h"
#include "GameState.h"
#include "Model.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>

using json = nlohmann::json;

// Single global game — fine for a solo demo (one game at a time).
static Board g_board;
static GameState g_game;
static Model g_model;
static std::mutex g_mutex;

static json statusJson()
{
    return json{
        {"turn", g_game.getCurrentTurn() ? "white" : "black"},
        {"gameOver", g_game.gameOver},
        {"isCheck", g_game.getIsCheck()}
    };
}

int main()
{
    httplib::Server svr;

    // CORS — tighten the origin once you know your deployed frontend URL
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    svr.Post("/api/new-game", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_board = Board();
        g_game = GameState();
        res.set_content(statusJson().dump(), "application/json");
    });

    svr.Post("/api/move", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);

        json body;
        try {
            body = json::parse(req.body);
        } catch (...) {
            res.status = 400;
            res.set_content(json{{"error", "invalid json"}}.dump(), "application/json");
            return;
        }

        int fromFile = body.value("fromFile", -1);
        int fromRank = body.value("fromRank", -1);
        int toFile   = body.value("toFile", -1);
        int toRank   = body.value("toRank", -1);
        int promotion = body.value("promotion", 0);

        bool userMoveOk = g_game.tryMakeMove(
            g_board, fromFile, fromRank, toFile, toRank,
            static_cast<Board::PieceType>(promotion)
        );

        json response;
        response["userMoveOk"] = userMoveOk;
        response.update(statusJson());

        if (!userMoveOk) {
            res.set_content(response.dump(), "application/json");
            return;
        }

        if (!g_game.gameOver) {
            Move aiMove = g_model.getMove(g_board, g_game);
            bool aiMoveOk = g_game.tryMakeMove(
                g_board, aiMove.startFile, aiMove.startRank,
                aiMove.endFile, aiMove.endRank, aiMove.promotionPiece
            );

            if (aiMoveOk) {
                response["aiMove"] = {
                    {"fromFile", aiMove.startFile},
                    {"fromRank", aiMove.startRank},
                    {"toFile", aiMove.endFile},
                    {"toRank", aiMove.endRank},
                    {"promotion", static_cast<int>(aiMove.promotionPiece)}
                };
            } else {
                // Shouldn't normally happen since getMove only picks from legal moves
                response["aiMoveError"] = true;
            }
            response.update(statusJson()); // refresh turn/gameOver/check after AI move
        }

        res.set_content(response.dump(), "application/json");
    });

    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("ok", "text/plain");
    });

    std::cout << "Chess server listening on http://0.0.0.0:8080\n";
    svr.listen("0.0.0.0", 8080);
}