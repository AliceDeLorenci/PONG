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
    constexpr float PADDING = 10.0f;
    constexpr int32_t DIV_SIZE = 6;
    constexpr int32_t BORDER = 1;
    constexpr int FAIL = 1;

    enum PlayerNum { PlayerOne,
                     PlayerTwo };

    class Pong : public olc::PixelGameEngine {
     private:
        std::array<std::unique_ptr<Player::Player>, 2> players;
        std::unique_ptr<Ball::Ball> ball;
        std::array<int, 2> score;

#if SERVER
        Network::Server::Server server;

        bool create;                    // Flag that indicates whether the connection setup was completed
        std::thread setup_connections;  // Thread responsible for accepting clients
        void SetUpConnections();        // Method used by the setup_connections threas
#elif CLIENT
        Network::Client::Client client;
#endif

#if SERVER || CLIENT
     public:
        Pong(const std::string& ServerIp = Network::LOCALHOST, const std::string& UDPServerPort = Network::DEFAULT_UDP_PORT, const std::string& TCPServerPort = Network::DEFAULT_TCP_PORT);
        bool OnUserDestroy() override;  // called when the user clicks the exit button
#else
     public:
        Pong();
#endif

     private:
        void DrawDivision();
        void Init();

     public:
        bool OnUserCreate() override;
        bool OnUserUpdate(float) override;
        virtual ~Pong();

#if CLIENT
        void UpdatePositions();
#endif
    };
}  // namespace Pong

#endif  // !PONG_H
