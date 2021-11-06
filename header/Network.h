#if defined CLIENT || defined SERVER
#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <iostream>

namespace Pong::Network {
    constexpr int FAIL = 1;
    constexpr int MAXLINE = 1024;
    static constexpr const char* LOCALHOST = "127.0.0.1";
    static constexpr const char* DEFAULT_UDP_PORT = "1234";
    static constexpr const char* DEFAULT_TCP_PORT = "1235";

    enum Connections { UDP,
                       TCP };

    int ConvertPort(const std::string&);
    in_addr_t ConvertIp(const std::string&);
}  // namespace Pong::Network

namespace Pong::Network::GameInfo {
    struct GameInfo {
        int xPlayer1;
        int yPlayer1;
        int xPlayer2;
        int yPlayer2;
        int xBall;
        int yBall;
        int scorePlayer1;
        int scorePlayer2;

        void Serialize();
        void Deserialize();
    };
}  // namespace Pong::Network::GameInfo

#endif  // !NETWORK_H
#endif  // CLIENT || SERVER