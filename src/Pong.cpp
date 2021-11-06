#include "Pong.h"

#include "spdlog/spdlog.h"

namespace Pong {
#if SERVER
    Pong::Pong(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) : server(ServerIp, UDPServerPort, TCPServerPort) {
        sAppName = "Pong -- Server";
    }

    bool Pong::OnUserCreate() {
        bool create = false;

        setup_connections = std::thread(&Pong::SetUpConnections, this);
        setup_connections.detach();

        return true;
    }

    void Pong::SetUpConnections() {
        for (int client = Network::Server::ClientOne; client <= Network::Server::ClientTwo; client++) {
            spdlog::info("Waiting for player {}...", client + 1);
            if (server.AcceptClient(client) == FAIL)
                return;
            spdlog::info("Connected to player {}!", client + 1);
        }
        spdlog::info("Starting the game...");
        server.StartListeningUDP();

        Init();
        create = true;
        spdlog::info("Game started!");
    }

    bool Pong::OnUserUpdate(float fElapsedTime) {
        // When OnUserUpdate returns false the PixelGameEngine exits (calls on UserDestroy)
        if (server.GetQuit()) {
            spdlog::info("Starting to close...");
            return false;
        }

        Clear(olc::BLACK);
        // Only start the game after both clients connected
        if (!create) {
            std::string status = "Waiting for Players";
            DrawString(ScreenWidth() / 2 - GetTextSize(status).x, ScreenHeight() / 2 - GetTextSize(status).y, status, olc::GREEN, 2);
            return true;
        }

        // User input
        if (server.GetKey(Network::Server::W))
            players[PlayerOne]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::S))
            players[PlayerOne]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::UP))
            players[PlayerTwo]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::DOWN))
            players[PlayerTwo]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        // Moves the ball
        ball->Move(fElapsedTime);

        // Checks whether the ball colided with the players
        ball->CheckCollision(players[PlayerOne]->Position(), players[PlayerTwo]->Position(), score);

        server.msg.xPlayer1 = players[PlayerOne]->Position().x;
        server.msg.yPlayer1 = players[PlayerOne]->Position().y;
        server.msg.xPlayer2 = players[PlayerTwo]->Position().x;
        server.msg.yPlayer2 = players[PlayerTwo]->Position().y;
        server.msg.xBall = ball->Position().x;
        server.msg.yBall = ball->Position().y;
        server.msg.scorePlayer1 = score[PlayerOne];
        server.msg.scorePlayer2 = score[PlayerTwo];

        server.SendPosition(Network::Server::ClientOne);
        server.SendPosition(Network::Server::ClientTwo);

        std::string playersIp = server.GetClientsIp();
        DrawString(ScreenWidth() / 2 - GetTextSize(playersIp).x, int32_t(PADDING), playersIp, olc::GREEN, 2);

        std::string scoreLine = "Player 1: " + std::to_string(score[PlayerOne]) + '\n' + "Player 2: " + std::to_string(score[PlayerTwo]);
        DrawString(ScreenWidth() / 2 - GetTextSize(scoreLine).x, ScreenHeight() - GetTextSize(scoreLine).y * 2 - int32_t(PADDING), scoreLine, olc::GREEN, 2);

        std::string status = "Game Running";
        DrawString(ScreenWidth() / 2 - GetTextSize(status).x, ScreenHeight() / 2 - GetTextSize(status).y, status, olc::GREEN, 2);

        return true;
    }

    /***
	 * Called when the user clicks the exit button or when a client disconnected. 
	 * Announces to the clients that the game has ended.
	***/
    bool Pong::OnUserDestroy() {
        spdlog::info("Disconnecting...");
        server.QuitListener();

        for (int client = Network::Server::ClientOne; client <= Network::Server::ClientTwo; client++) {
            if (server.IsClientConnected(client)) {
                spdlog::info("Asking player {} to disconnect...", client + 1);
                server.AnnounceEnd(client);
                spdlog::info("Player {} disconnected!", client + 1);
            }
        }

        return true;
    }

#elif CLIENT
    Pong::Pong(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) : client(ServerIp, UDPServerPort, TCPServerPort) {
        sAppName = "Pong -- Client";
    }

    bool Pong::OnUserCreate() {
        if (client.Connect() == FAIL)
            exit(EXIT_FAILURE);

        client.StartListening();

        Init();

        client.msg.xPlayer1 = players[PlayerOne]->Position().x;
        client.msg.yPlayer1 = players[PlayerOne]->Position().y;
        client.msg.xPlayer2 = players[PlayerTwo]->Position().x;
        client.msg.yPlayer2 = players[PlayerTwo]->Position().y;
        client.msg.xBall = ball->Position().x;
        client.msg.yBall = ball->Position().y;
        client.msg.scorePlayer1 = 0;
        client.msg.scorePlayer2 = 0;

        return true;
    }

    bool Pong::OnUserUpdate(float fElapsedTime) {
        // When OnUserUpdate returns false the PixelGameEngine exits
        if (client.GetQuit()) {
            spdlog::info("Quitting...");
            return false;
        }

        // User input
        client.SetKey(Network::Client::UP, GetKey(olc::Key::UP).bHeld);
        client.SetKey(Network::Client::DOWN, GetKey(olc::Key::DOWN).bHeld);
        client.SendKeys();

        UpdatePositions();

        // Clears the screen
        Clear(olc::BLACK);

        // Display the scenario
        DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);
        DrawDivision();

        // Display the score
        std::string scoreLine[2] = {std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo])};
        DrawString(ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerOne]).x, int32_t(PADDING), scoreLine[PlayerOne], olc::GREEN, 2);
        DrawString(3 * ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerTwo]).x, int32_t(PADDING), scoreLine[PlayerTwo], olc::GREEN, 2);

        // Displays entities
        players[PlayerOne]->Draw();
        players[PlayerTwo]->Draw();
        ball->Draw();

        return true;
    }

    void Pong::UpdatePositions() {
        players[PlayerOne]->SetPosition(client.msg.xPlayer1, client.msg.yPlayer1);
        players[PlayerTwo]->SetPosition(client.msg.xPlayer2, client.msg.yPlayer2);
        ball->SetPosition(client.msg.xBall, client.msg.yBall);
        score[PlayerOne] = client.msg.scorePlayer1;
        score[PlayerTwo] = client.msg.scorePlayer2;
    }

    /***
	 * Called when the user clicks the exit button or the server exits. 
	 * Announces to the server that the game has ended if the former is true.
	***/
    bool Pong::OnUserDestroy() {
        spdlog::info("Disconnecting...");
        client.QuitListener();

        if (!client.GetServerQuit()) {  // the quitting action didn't start with the server
            spdlog::info("Telling the server to quit...");
            client.AnnounceEnd();
            spdlog::info("Told server to quit!");
        }
        return true;
    }

#else
    Pong::Pong() {
        sAppName = "Pong";
    }

    bool Pong::OnUserCreate() {
        Init();

        spdlog::info("Game started! Good luck and have fun!");
        return true;
    }

    bool Pong::OnUserUpdate(float fElapsedTime) {
        // User input
        if (GetKey(olc::Key::W).bHeld)
            players[PlayerOne]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::S).bHeld)
            players[PlayerOne]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::UP).bHeld)
            players[PlayerTwo]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::DOWN).bHeld)
            players[PlayerTwo]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        // Moves the ball
        ball->Move(fElapsedTime);

        // Checks whether the ball colided with the players
        ball->CheckCollision(players[PlayerOne]->Position(), players[PlayerTwo]->Position(), score);

        // Clears the screen
        Clear(olc::BLACK);

        // Display the scenario
        DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);
        DrawDivision();

        // Display the score
        std::string scoreLine[2] = {std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo])};
        DrawString(ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerOne]).x, int32_t(PADDING), scoreLine[PlayerOne], olc::GREEN, 2);
        DrawString(3 * ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerTwo]).x, int32_t(PADDING), scoreLine[PlayerTwo], olc::GREEN, 2);

        // Displays entities
        players[PlayerOne]->Draw();
        players[PlayerTwo]->Draw();
        ball->Draw();

        return true;
    }
#endif

    Pong::~Pong() {}

    void Pong::Init() {
        players[PlayerOne] = std::make_unique<Player::Player>(Player::PlayerOne, *this);
        players[PlayerTwo] = std::make_unique<Player::Player>(Player::PlayerTwo, *this);

        ball = std::make_unique<Ball::Ball>(*this);

        score = {0, 0};
    }

    void Pong::DrawDivision() {
        int n = int(float(ScreenHeight()) / float(DIV_SIZE));
        int x = ScreenWidth() / 2 - DIV_SIZE / 4;
        int y = 0;
        for (int i = 0; i < n; i += 2) {
            FillRect(x, y, DIV_SIZE / 2, DIV_SIZE, olc::WHITE);
            y += 2 * DIV_SIZE;
        }
    }
}  // namespace Pong