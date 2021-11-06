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

        // UDP
        // Client: "I am <player number>"

        // Accepts a UDP client and saves its address for future reference
        while (true) {
            char buffer[MAXLINE];
            unsigned len = sizeof(&(UDP_clients[clientNum]));
            int n = recvfrom(sockets[UDP], (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&UDP_clients[clientNum], &len);
            buffer[n] = '\0';

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

            if (strncmp(INCOMING_KEYS, buffer, strlen(INCOMING_KEYS)) == 0) {
                char type[5];
                int isPlayerOne;
                int up;
                int down;

                sscanf(buffer, "%s %d %d %d", type, &isPlayerOne, &up, &down);

                if (isPlayerOne) {
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

                if (strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0) {
                    spdlog::info("Client {} disconnected", client);

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
        msg.Serialize();
        if (sendto(sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*)&UDP_clients[client_num], len) < 0) {
            return 1;
        }
        msg.Deserialize();

        return 0;
    }

    int Server::AnnounceEnd(int client_num) {
        char client_msg[MAXLINE];
        strcpy(client_msg, USER_DESTROY);

        if (send(TCP_clients[client_num], (const char*)client_msg, strlen(client_msg), 0) < 0) {
            spdlog::error("Failed to send quit message to a client!");
            if (errno) perror("");
            return 1;
        }

        return 0;
    }

    bool Server::GetKey(int key_num) { return keys[key_num]; }
    bool Server::GetQuit() { return quit; }
    bool Server::GetClientQuit() { return client_quit; }
    void Server::QuitListener() { quit_listener = true; }
    bool Server::IsClientConnected(int client) { return (TCP_clients[client] < 0) ? false : true; }
}  // namespace Pong::Network::Server
#endif