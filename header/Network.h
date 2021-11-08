#if defined CLIENT || defined SERVER
#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <iostream>

namespace Pong::Network {
    constexpr int FAIL = 1;
    constexpr int MAXLINE = 1024;       // maximum buffer size for the messages sent and received through the network
    constexpr int TIMEOUT = 1000;       // connection timeout interval

    // default IP address and ports
    static constexpr const char* LOCALHOST = "127.0.0.1";
    static constexpr const char* DEFAULT_UDP_PORT = "1234";
    static constexpr const char* DEFAULT_TCP_PORT = "1235";

    enum Connections { UDP,
                       TCP };

    // IP address and port validation functionalities
    int ConvertPort(const std::string&);
    in_addr_t ConvertIp(const std::string&);

}  // namespace Pong::Network

namespace Pong::Network::GameInfo {
    // struct sent by the server to the client containing the current game configuration
    struct GameInfo {
        int xPlayer1;
        int yPlayer1;
        int xPlayer2;
        int yPlayer2;
        int xBall;
        int yBall;
        int scorePlayer1;
        int scorePlayer2;

        // convert to and from network byte order
        void Serialize();
        void Deserialize();
    };
}  // namespace Pong::Network::GameInfo

#endif  // !NETWORK_H
#endif  // CLIENT || SERVER