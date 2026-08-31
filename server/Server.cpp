#include "BoardState.h"
#include "GameState.h"
#include "Model.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <iostream>
#include <windows.h>

using json = nlohmann::json;

// Single global game — fine for a solo demo (one game at a time).
static Board g_board;
static GameState g_game;
static Model g_model;
static std::mutex g_mutex;
static httplib::Server g_svr;

static json statusJson()
{
    return json{
        {"turn", g_game.getCurrentTurn() ? "white" : "black"},
        {"gameOver", g_game.gameOver},
        {"isCheck", g_game.getIsCheck()}
    };
}

BOOL WINAPI consoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType) {
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            std::cout << "Shutting down server...\n";
            g_svr.stop();
            return TRUE;
        default:
            return FALSE;
    }
}

int main()
{
    SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

    // CORS — tighten the origin once you know your deployed frontend URL
    g_svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    g_svr.Post("/api/new-game", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_board = Board();
        g_game = GameState();
        res.set_content(statusJson().dump(), "application/json");
    });

    g_svr.Post("/api/move", [](const httplib::Request& req, httplib::Response& res) {
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

        try {
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

                std::cout << "AI chose move: (" << aiMove.startFile << "," << aiMove.startRank
                        << ") -> (" << aiMove.endFile << "," << aiMove.endRank << ")\n";

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
                    response["aiMoveError"] = true;
                    std::cerr << "WARNING: AI proposed an illegal move!\n";
                }
                response.update(statusJson());
            }

            res.set_content(response.dump(), "application/json");
        }
        catch (const c10::Error& e) {
            std::cerr << "Torch error during move handling: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(json{{"error", "model inference failed"}}.dump(), "application/json");
        }
        catch (const std::exception& e) {
            std::cerr << "Error during move handling: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    g_svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("ok", "text/plain");
    });

    std::cout << "Chess server listening on http://0.0.0.0:8080\n";
    g_svr.listen("0.0.0.0", 8080);
}