#pragma once
#ifndef PONG_H
#define PONG_H

#include <array>

#include "Ball.h"
#include "Client.h"
#include "Player.h"
#include "Server.h"
#include "olcPixelGameEngine.h"

namespace Pong {
    constexpr float PADDING = 10.0f;            // screen padding
    constexpr int32_t DIV_SIZE = 6;             // screen division
    constexpr int32_t BORDER = 1;               // screen border
    constexpr int FAIL = 1;

    enum PlayerNum { PlayerOne,
                     PlayerTwo };

    class Pong : public olc::PixelGameEngine {
     private:
        // game entities and score
        std::array<std::unique_ptr<Player::Player>, 2> players;         
        std::unique_ptr<Ball::Ball> ball;                               
        std::array<int, 2> score;

/**
 * Variables unique to the server's game engine.
 */
#if SERVER
        Network::Server::Server server;

        bool create;                    // Flag that indicates whether the connection setup was completed
        std::thread setup_connections;  // Thread responsible for accepting clients
        void SetUpConnections();        // Method used by the setup_connections thread

/**
 * Variables unique to the client's game engine.
 */
#elif CLIENT
        Network::Client::Client client;
#endif

/**
 * Methods common to both clients and server.
 */
#if SERVER || CLIENT
     public:
        Pong(const std::string& ServerIp = Network::LOCALHOST, const std::string& UDPServerPort = Network::DEFAULT_UDP_PORT, const std::string& TCPServerPort = Network::DEFAULT_TCP_PORT);
        bool OnUserDestroy() override;          // called when the user clicks the exit button or OnUserUpdate returns false

/**
 * Methods unique to the offline game build.
 */
#else
     public:
        Pong();
#endif

     private:
        void DrawDivision();
        void Init();                            // initializes the game entities

     public:
        bool OnUserCreate() override;           // called after the instantiation of a Pong::Pong object
        bool OnUserUpdate(float) override;      // called at each new frame
        virtual ~Pong();

#if CLIENT
        void UpdatePositions();                 // update game entities' positions
#endif
    };
}  // namespace Pong

#endif  // !PONG_H
