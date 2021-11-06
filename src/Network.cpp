#if defined CLIENT || defined SERVER
#include "Network.h"

#include "spdlog/spdlog.h"

namespace Pong::Network {
    int ConvertPort(const std::string& port) {
        int ret_port;

        if (!port.empty() && port.find_first_not_of("0123456789") == std::string::npos) {  // if port is a number
            ret_port = std::stoi(port);
            if (ret_port <= 0 || ret_port > 65535) {
                spdlog::error("Port must be in range (0, 65535]!");
                if (errno) perror("");
                exit(FAIL);
            }
        } else {
            spdlog::error("Port must be a number!");
            if (errno) perror("");
            exit(FAIL);
        }
        return ret_port;
    }

    in_addr_t ConvertIp(const std::string& ip) {
        in_addr_t convertedIp;
        auto errCode = inet_pton(AF_INET, ip.c_str(), (void*)&convertedIp);
        if (errCode == 0) {
            spdlog::error("Invalid IP address!");
            if (errno) perror("");
            exit(FAIL);
        } else if (errCode == -1) {
            spdlog::error("inet_pton error!");
            if (errno) perror("");
            exit(FAIL);
        }
        return convertedIp;
    }
}  // namespace Pong::Network

namespace Pong::Network::GameInfo {
    void GameInfo::Serialize() {
        xPlayer1 = htonl(xPlayer1);
        yPlayer1 = htonl(yPlayer1);
        xPlayer2 = htonl(xPlayer2);
        yPlayer2 = htonl(yPlayer2);
        xBall = htonl(xBall);
        yBall = htonl(yBall);
        scorePlayer1 = htonl(scorePlayer1);
        scorePlayer2 = htonl(scorePlayer2);
    }
    void GameInfo::Deserialize() {
        xPlayer1 = ntohl(xPlayer1);
        yPlayer1 = ntohl(yPlayer1);
        xPlayer2 = ntohl(xPlayer2);
        yPlayer2 = ntohl(yPlayer2);
        xBall = ntohl(xBall);
        yBall = ntohl(yBall);
        scorePlayer1 = ntohl(scorePlayer1);
        scorePlayer2 = ntohl(scorePlayer2);
    }
}  // namespace Pong::Network::GameInfo

#endif