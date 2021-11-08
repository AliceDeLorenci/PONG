#include "Pong.h"

#include "spdlog/spdlog.h"

namespace Pong {

/**
 * Server's game engine.
 */
#if SERVER
    Pong::Pong(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) : server(ServerIp, UDPServerPort, TCPServerPort) {
        sAppName = "Pong -- Server";
    }

    /**
     * Function called after the instantiation of the Pong class, responsible for setting up the conections.
     */
    bool Pong::OnUserCreate() {
        bool create = false;

        // the connections are set up by a thread so that the program doesn't block while
        // waiting for the clients
        setup_connections = std::thread(&Pong::SetUpConnections, this);
        setup_connections.detach();

        return true;
    }

    /**
     * Sets up the UDP and TCP connections with the clients.
     */
    void Pong::SetUpConnections() {
        for (int client = Network::Server::ClientOne; client <= Network::Server::ClientTwo; client++) {
            spdlog::info("Waiting for player {}...", client + 1);
            if (server.AcceptClient(client) == FAIL)    // connect to clients
                return;
            spdlog::info("Connected to player {}!", client + 1);
        }
        spdlog::info("Starting the game...");
        server.StartListeningUDP();     // start listening to messages sent by the clients through the UDP connection

        Init();     // configures the game
        create = true;
        spdlog::info("Game started!");
    }

    /**
     * Called at each new frame. Manages game processing.
     */
    bool Pong::OnUserUpdate(float fElapsedTime) {
        
        // if the game has ended...
        if (server.GetQuit()) {
            spdlog::info("Starting to close...");
            return false;   // when OnUserUpdate returns false the PixelGameEngine exits (calls on UserDestroy)
        }

        Clear(olc::BLACK);

        // only start the game after both clients connected
        if (!create) {
            std::string status = "Waiting for Players";
            DrawString(ScreenWidth() / 2 - GetTextSize(status).x, ScreenHeight() / 2 - GetTextSize(status).y, status, olc::GREEN, 2);
            return true;
        }

        // get user input and move the players accordingly
        if (server.GetKey(Network::Server::W))
            players[PlayerOne]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::S))
            players[PlayerOne]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::UP))
            players[PlayerTwo]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (server.GetKey(Network::Server::DOWN))
            players[PlayerTwo]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        // moves the ball
        ball->Move(fElapsedTime);

        // checks whether the ball colided with the players
        ball->CheckCollision(players[PlayerOne]->Position(), players[PlayerTwo]->Position(), score);

        // packs the game configuration in a message
        server.msg.xPlayer1 = players[PlayerOne]->Position().x;
        server.msg.yPlayer1 = players[PlayerOne]->Position().y;
        server.msg.xPlayer2 = players[PlayerTwo]->Position().x;
        server.msg.yPlayer2 = players[PlayerTwo]->Position().y;
        server.msg.xBall = ball->Position().x;
        server.msg.yBall = ball->Position().y;
        server.msg.scorePlayer1 = score[PlayerOne];
        server.msg.scorePlayer2 = score[PlayerTwo];
        
        // sends the game configuration to the clients through the UDP conection
        server.SendPosition(Network::Server::ClientOne);
        server.SendPosition(Network::Server::ClientTwo);

        // prints the players IP addresses and their scores on the screen
        std::string playersIp = server.GetClientsIp();
        DrawString(ScreenWidth() / 2 - GetTextSize(playersIp).x, int32_t(PADDING), playersIp, olc::GREEN, 2);

        std::string scoreLine = "Player 1: " + std::to_string(score[PlayerOne]) + '\n' + "Player 2: " + std::to_string(score[PlayerTwo]);
        DrawString(ScreenWidth() / 2 - GetTextSize(scoreLine).x, ScreenHeight() - GetTextSize(scoreLine).y * 2 - int32_t(PADDING), scoreLine, olc::GREEN, 2);

        std::string status = "Game Running";
        DrawString(ScreenWidth() / 2 - GetTextSize(status).x, ScreenHeight() / 2 - GetTextSize(status).y, status, olc::GREEN, 2);

        return true;
    }

    /***
	 * Called when the game ends, both when the user clicks the server's exit button 
     * and when a client disconnects (or his connection times out). 
	 * Announces to the clients that the game has ended.
	***/
    bool Pong::OnUserDestroy() {
        spdlog::info("Disconnecting...");
        server.QuitListener();  // terminate the threads listening to the UDP and TCP connection

        for (int client = Network::Server::ClientOne; client <= Network::Server::ClientTwo; client++) {
            if (server.IsClientConnected(client)) {     // if the client is connected...
                spdlog::info("Asking player {} to disconnect...", client + 1);
                server.AnnounceEnd(client);     // ...announce that the game has ended
                spdlog::info("Player {} disconnected!", client + 1);
            }
        }

        return true;
    }

/**
 * Client's game engine.
 */
#elif CLIENT
    Pong::Pong(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) : client(ServerIp, UDPServerPort, TCPServerPort) {
        sAppName = "Pong -- Client";
    }

    /**
     * Function called after the instantiation of the Pong class, responsible for connecting to the server.
     */
    bool Pong::OnUserCreate() {
        if (client.Connect() == FAIL)   // connects to the server
            exit(EXIT_FAILURE);

        client.StartListening();    // start listening for messages sent by the server

        Init();     // configures the game

        // game start configuration
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

    /**
     * Called at each new frame. Manages game processing.
     */
    bool Pong::OnUserUpdate(float fElapsedTime) {
        
        // if the game has ended...
        if (client.GetQuit()) {
            spdlog::info("Quitting...");
            return false;   // when OnUserUpdate returns false the PixelGameEngine exits
        }

        // reads user input
        client.SetKey(Network::Client::UP, GetKey(olc::Key::UP).bHeld);
        client.SetKey(Network::Client::DOWN, GetKey(olc::Key::DOWN).bHeld);
        // sends user input to the server through the UDP connection
        client.SendKeys();

        // updates the game configuration according to the message received from the server 
        UpdatePositions();

        // xlears the screen
        Clear(olc::BLACK);

        // display the scenario
        DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);
        DrawDivision();

        // display the score
        std::string scoreLine[2] = {std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo])};
        DrawString(ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerOne]).x, int32_t(PADDING), scoreLine[PlayerOne], olc::GREEN, 2);
        DrawString(3 * ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerTwo]).x, int32_t(PADDING), scoreLine[PlayerTwo], olc::GREEN, 2);

        // displays entities
        players[PlayerOne]->Draw();
        players[PlayerTwo]->Draw();
        ball->Draw();

        return true;
    }
    
    /**
     * Updates the player configuration (positioning of th eplayers and the ball and the current scores).
     */
    void Pong::UpdatePositions() {
        players[PlayerOne]->SetPosition(client.msg.xPlayer1, client.msg.yPlayer1);
        players[PlayerTwo]->SetPosition(client.msg.xPlayer2, client.msg.yPlayer2);
        ball->SetPosition(client.msg.xBall, client.msg.yBall);
        score[PlayerOne] = client.msg.scorePlayer1;
        score[PlayerTwo] = client.msg.scorePlayer2;
    }

    /***
	 * Called when the game ends, both when the user clicks the client's exit button 
     * and when a server disconnects (or his connection times out). 
	 * Announces to the clients that the game has ended.
	***/
    bool Pong::OnUserDestroy() {
        spdlog::info("Disconnecting...");
        client.QuitListener();      // terminate the threads listening to the UDP and TCP connection

        if (!client.GetServerQuit()) {  // the quitting action started by self
            spdlog::info("Telling the server to quit...");
            client.AnnounceEnd();   // accounce to the server that the game has ended
            spdlog::info("Told server to quit!");
        }
        return true;
    }

/**
 * Offline game engine.
 */
#else
    Pong::Pong() {
        sAppName = "Pong";
    }

    /**
     * Called after instantiation of the Pong class, responsible for setting up the game.
     */
    bool Pong::OnUserCreate() {
        Init();

        spdlog::info("Game started! Good luck and have fun!");
        return true;
    }

    /**
     * Called at each new frame. Manages game processing.
     */
    bool Pong::OnUserUpdate(float fElapsedTime) {
        // user input
        if (GetKey(olc::Key::W).bHeld)
            players[PlayerOne]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::S).bHeld)
            players[PlayerOne]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::UP).bHeld)
            players[PlayerTwo]->Move(olc::vf2d(0, -Player::Player::speed * fElapsedTime));

        if (GetKey(olc::Key::DOWN).bHeld)
            players[PlayerTwo]->Move(olc::vf2d(0, +Player::Player::speed * fElapsedTime));

        // moves the ball
        ball->Move(fElapsedTime);

        // checks whether the ball colided with the players
        ball->CheckCollision(players[PlayerOne]->Position(), players[PlayerTwo]->Position(), score);

        // clears the screen
        Clear(olc::BLACK);

        // display the scenario
        DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);
        DrawDivision();

        // display the score
        std::string scoreLine[2] = {std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo])};
        DrawString(ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerOne]).x, int32_t(PADDING), scoreLine[PlayerOne], olc::GREEN, 2);
        DrawString(3 * ScreenWidth() / 4 - GetTextSize(scoreLine[PlayerTwo]).x, int32_t(PADDING), scoreLine[PlayerTwo], olc::GREEN, 2);

        // displays entities
        players[PlayerOne]->Draw();
        players[PlayerTwo]->Draw();
        ball->Draw();

        return true;
    }
#endif

    Pong::~Pong() {}

    /**
     * Initializes game entities.
     */
    void Pong::Init() {
        players[PlayerOne] = std::make_unique<Player::Player>(Player::PlayerOne, *this);
        players[PlayerTwo] = std::make_unique<Player::Player>(Player::PlayerTwo, *this);

        ball = std::make_unique<Ball::Ball>(*this);

        score = {0, 0};
    }

    /**
     * Draws the dashed screen division.
     */
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