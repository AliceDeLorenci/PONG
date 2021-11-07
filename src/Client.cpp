#if CLIENT
#include "Client.h"

#include <errno.h>

#include "spdlog/spdlog.h"

namespace Pong::Network::Client {
    Client::Client(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) {
        ports[UDP] = ConvertPort(UDPServerPort);
        ports[TCP] = ConvertPort(TCPServerPort);
        auto convertedIp = ConvertIp(ServerIp);

        // Server information
        for (int TYPE = UDP; TYPE <= TCP; TYPE++) {
            memset(&server_addr[TYPE], 0, sizeof(server_addr[TYPE]));
            server_addr[TYPE].sin_family = AF_INET;
            server_addr[TYPE].sin_port = htons(ports[TYPE]);
            server_addr[TYPE].sin_addr.s_addr = convertedIp;
        }

        memset(&msg, 0, sizeof(msg));

        server_quit = false;
        quit_listener = false;
        quit = false;
    }
    Client::~Client() {}

    int Client::Connect() {
        spdlog::info("Trying to connect to the server...");
        // TCP
        // Server: "<player number>"
        sockets[TCP] = socket(AF_INET, SOCK_STREAM, 0);

        if (connect(sockets[TCP], (struct sockaddr*)&server_addr[TCP], sizeof(server_addr[TCP])) == -1) {
            spdlog::error("Failed to stablish a TCP connection. The server may be disconnected.");
            if (errno) perror("");
            return 1;
        }

        // Receive player number
        char incoming_msg[MAXLINE];
        int n = recv(sockets[TCP], incoming_msg, MAXLINE, 0);
        incoming_msg[n] = '\0';
        spdlog::trace("Received from server using TCP {}", incoming_msg);
        player_num = atoi(incoming_msg);

        // UDP
        // Client: "I am <player number>"
        sockets[UDP] = socket(AF_INET, SOCK_DGRAM, 0);
        char buffer[MAXLINE];
        sprintf(buffer, "I AM %d", player_num);

        // nonblocking UDP socket
        int dontblock = 1;
        auto rc = ioctl(sockets[UDP], FIONBIO, (char *) &dontblock);    

        // return 1;    // testing UDP connection timeout

        socklen_t len = sizeof(server_addr[UDP]);
        spdlog::trace("Sending authentication to the server using UDP: {}", buffer);
        if (sendto(sockets[UDP], buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr*)&server_addr[UDP], len) < 0) {
            spdlog::error("Failed to authenticate to the server using UDP!");
            if (errno) perror("");
            return 1;
        }
        spdlog::trace("Sent authentication to the server using UDP");

        spdlog::info("Connected to the server as player {}!", player_num + 1);
        if (player_num == 0)
            spdlog::info("Waiting for player 2...");

        return 0;
    }

    void Client::StartListening() {
        thread_listen[UDP] = std::thread(&Client::ListenUDP, this);
        thread_listen[UDP].detach();

        thread_listen[TCP] = std::thread(&Client::ListenTCP, this);
        thread_listen[TCP].detach();
    }

    void Client::ListenUDP() {
        static bool gameStarted = false;

        using namespace std::chrono;
        high_resolution_clock::time_point last_contact;

        while (!quit_listener) {

            // connection timeout
            if( gameStarted ){
                if(  duration_cast<milliseconds>( high_resolution_clock::now() - last_contact ).count() > TIMEOUT ){
                    spdlog::info("Server connection timed out");
                    quit_listener = true;
                    quit = true;
                    return;
                }
            }

            socklen_t len = sizeof(server_addr[UDP]);
            int n = recvfrom(sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_WAITALL, (struct sockaddr*)&server_addr[UDP], &len);
            if( n == -1 )
                continue;
            
            if (!gameStarted) {
                spdlog::info("Game started! Good luck and have fun!");
                gameStarted = true;
            }

            last_contact = high_resolution_clock::now();
            msg.Deserialize();
            spdlog::trace("Received from server using UDP: {} {} {} {} {} {} {} {}", msg.xPlayer1, msg.yPlayer1, msg.xPlayer2, msg.yPlayer2, msg.xBall, msg.yBall, msg.scorePlayer1, msg.scorePlayer2);
        }
    }

    void Client::ListenTCP() {
        fcntl(sockets[TCP], F_SETFL, fcntl(sockets[TCP], F_GETFL) | O_NONBLOCK);
        while (!quit_listener) {
            char buffer[MAXLINE];
            int n = recv(sockets[TCP], buffer, MAXLINE, 0);
            if (n == -1) continue;
            buffer[n] = '\0';
            spdlog::trace("Received from server using TCP: {}", buffer);

            if (strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0) {
                spdlog::info("Server disconnected");

                quit = true;
                server_quit = true;
                quit_listener = true;
            }
        }
    }

    int Client::SendKeys() {
        // build string
        char client_msg[MAXLINE];
        sprintf(client_msg, "KEYS %d %d %d", player_num, keys[UP], keys[DOWN]);

        socklen_t len = sizeof(server_addr[UDP]);
        spdlog::trace("Sending keys to server using UDP: {}", client_msg);
        if (sendto(sockets[UDP], (const char*)client_msg, strlen(client_msg), MSG_CONFIRM, (const struct sockaddr*)&server_addr[UDP], len) < 0) {
            spdlog::error("Failed to send key presses!");
            if (errno) perror("");
            return 1;
        }
        spdlog::trace("Successfully sent keys to server using UDP");
        return 0;
    }

    void Client::SetKey(int key, bool held) {
        keys[key] = held;
    }

    int Client::AnnounceEnd() {
        // build string
        char client_msg[MAXLINE];
        strcpy(client_msg, USER_DESTROY);
        spdlog::trace("Asking server to exit with TCP: {}", client_msg);

        if (send(sockets[TCP], (const char*)client_msg, strlen(client_msg), 0) < 0) {
            spdlog::error("Failed to send quit msg to the server");
            if (errno) perror("");
            return 1;
        }
        spdlog::trace("Successfully asked server to quit using TCP");

        return 0;
    }

    bool Client::GetQuit() { return quit; }
    bool Client::GetServerQuit() { return server_quit; }
    void Client::QuitListener() {
        quit_listener = true;
        spdlog::info("Stopped listening to the server!");
    }
}  // namespace Pong::Network::Client
#endif  // CLIENT