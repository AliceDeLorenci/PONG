#if SERVER
#include "Server.h"

#include "spdlog/spdlog.h"

namespace Pong::Network::Server {
    Server::Server(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort)
        : ip(ServerIp), TCP_clients({-1, -1}), client_quit(false), quit_listener(false), quit(false) {
        CreateConnection(UDP, UDPServerPort);
        CreateConnection(TCP, TCPServerPort);
    }
    Server::~Server() {}

    int Server::CreateConnection(int TYPE, const std::string& ServerPort) {
        ports[TYPE] = ConvertPort(ServerPort);

        int socket_type = (TYPE == UDP) ? SOCK_DGRAM : SOCK_STREAM;

        // Allocate a socket for the server
        if ((sockets[TYPE] = socket(AF_INET, socket_type, 0)) < 0) {
            spdlog::error("Failed to create a server socket!");
            if (errno) perror("");
            exit(EXIT_FAILURE);
        }

        int allow = 1;
        if (setsockopt(sockets[TYPE], SOL_SOCKET, SO_REUSEADDR, &allow, sizeof(int)) < 0)  // Allows reuse of local address
            spdlog::error("Failed to allow the reuse of local address for server socket of type {}", TYPE);
            
        // Assign a port and IP address to the server socket
        auto convertedIp = ConvertIp(ip);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ports[TYPE]);
        addr.sin_addr.s_addr = convertedIp;
        memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

        // Bind socket to port
        if (bind(sockets[TYPE], (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            spdlog::error("Failed to bind a server socket!");
            if (errno) perror("");
            exit(FAIL);
        }

        if (TYPE == TCP) {
            // socket in passive open mode
            if (listen(sockets[TCP], 1) == -1) {
                spdlog::error("Could not prepare to accept connections on listen()");
                if (errno) perror("");
                return 1;
            }
        }

        return 0;
    }

    int Server::AcceptClient(int clientNum) {
        if ((TCP_clients[clientNum] = accept(sockets[TCP], 0, 0)) < 0) {
            spdlog::error("Failed to accept client!");
            if (errno) perror("");
            return 1;
        }

        // Says which player the client is
        const char* client_msg = std::to_string(clientNum).c_str();
        send(TCP_clients[clientNum], client_msg, strlen(client_msg), 0);
        spdlog::trace("Sent to client {} using TCP: {}", clientNum, client_msg);

        // UDP
        // Client: "I am <player number>"

        // Accepts a UDP client and saves its address for future reference
        while (true) {
            char buffer[MAXLINE];
            unsigned len = sizeof(&(UDP_clients[clientNum]));
            int n = recvfrom(sockets[UDP], (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&UDP_clients[clientNum], &len);
            buffer[n] = '\0';
            spdlog::trace("Received from a client using UDP: {}", buffer);

            if (!strncmp(buffer, NEW_UDP_CLIENT, strlen(NEW_UDP_CLIENT)))
                continue;

            if (atoi(&buffer[strlen(NEW_UDP_CLIENT) + 1]) != clientNum)
                continue;

            break;
        }

        // makes the socket non-blocking
        fcntl(TCP_clients[clientNum], F_SETFL, fcntl(TCP_clients[clientNum], F_GETFL) | O_NONBLOCK);

        if (clientNum == ClientOne) {
            thread_listen[TCP] = std::thread(&Server::ListenTCP, this);
            thread_listen[TCP].detach();
        }

        return 0;
    }

    void Server::StartListeningUDP() {
        thread_listen[UDP] = std::thread(&Server::ListenUDP, this);
        thread_listen[UDP].detach();
    }

    void Server::ListenUDP() {
        struct sockaddr_in client_addr;

        while (!quit_listener) {
            char buffer[MAXLINE];
            socklen_t len = sizeof(client_addr);
            memset(&client_addr, 0, sizeof(client_addr));
            int n = recvfrom(sockets[UDP], (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr, &len);
            buffer[n] = '\0';
            spdlog::trace("Received from a client using UDP: {}", buffer);

            if (strncmp(INCOMING_KEYS, buffer, strlen(INCOMING_KEYS)) == 0) {
                char type[5];
                int isPlayerTwo;
                int up;
                int down;

                sscanf(buffer, "%s %d %d %d", type, &isPlayerTwo, &up, &down);

                if (!isPlayerTwo) {
                    keys[W] = up;
                    keys[S] = down;
                } else {
                    keys[UP] = up;
                    keys[DOWN] = down;
                }
            }
        }
    }

    void Server::ListenTCP() {
        while (!quit_listener) {
            for (int client = ClientOne; client <= ClientTwo; client++) {
                // check if the TCP socket is connected and ready
                if (TCP_clients[client] != -1 && !(fcntl(TCP_clients[client], F_GETFL) & O_NONBLOCK)) continue;

                char buffer[MAXLINE];
                int n = recv(TCP_clients[client], buffer, MAXLINE, 0);
                if (n == -1) continue;
                buffer[n] = '\0';
                spdlog::trace("Received from client {} using TCP: {}", client, buffer);

                if (strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0) {
                    spdlog::info("Player {} disconnected", client + 1);

                    TCP_clients[client] = -1;
                    quit = true;
                    client_quit = true;
                    quit_listener = true;
                    break;
                }
            }
        }
    }

    int Server::SendPosition(int client_num) {
        socklen_t len = sizeof(UDP_clients[client_num]);
        spdlog::trace("Sending to client {} using TCP: {} {} {} {} {} {} {} {}", client_num, msg.xPlayer1, msg.yPlayer1, msg.xPlayer2, msg.yPlayer2, msg.xBall, msg.yBall, msg.scorePlayer1, msg.scorePlayer2);
        msg.Serialize();
        if (sendto(sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*)&UDP_clients[client_num], len) < 0) {
            spdlog::error("Could not send position to client {}!", client_num);
            return 1;
        }
        msg.Deserialize();
        spdlog::trace("Sent position to client {}", client_num);

        return 0;
    }

    int Server::AnnounceEnd(int client_num) {
        char client_msg[MAXLINE];
        strcpy(client_msg, USER_DESTROY);
        spdlog::trace("Sending to client {} using TCP: {}", client_num, client_msg);
        if (send(TCP_clients[client_num], (const char*)client_msg, strlen(client_msg), 0) < 0) {
            spdlog::error("Failed to send quit message to client {}!", client_num);
            if (errno) perror("");
            return 1;
        }
        spdlog::trace("Sent quit msg to client {}", client_num);

        return 0;
    }

    bool Server::GetKey(int key_num) { return keys[key_num]; }
    bool Server::GetQuit() { return quit; }
    bool Server::GetClientQuit() { return client_quit; }
    bool Server::IsClientConnected(int client) { return !(TCP_clients[client] < 0); }
    void Server::QuitListener() {
        quit_listener = true;
        spdlog::info("Stopped receiveing players commands!");
    }

    std::string Server::GetClientsIp() {
        std::string retval;
        for (int i = ClientOne; i <= ClientTwo; i++) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &UDP_clients[i], ip, INET_ADDRSTRLEN);
            retval += "Client " + std::to_string(i + 1) + ": " + ip + '\n';
        }
        return retval;
    }
}  // namespace Pong::Network::Server
#endif