#if CLIENT
#include "Client.h"

#include <errno.h>

#include "spdlog/spdlog.h"

/**
 * The gamers are clients. The client program only reads the user input and sends it to the server
 * and receives from the server the game configuration.
 * The server is responsible for all the game processing.
 */
namespace Pong::Network::Client {

    /**
     * Constructs a new Client::Client and saves the server address.
     */
    Client::Client(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) {
        // server ports
        ports[UDP] = ConvertPort(UDPServerPort);
        ports[TCP] = ConvertPort(TCPServerPort);
        auto convertedIp = ConvertIp(ServerIp);

        // server information
        for (int TYPE = UDP; TYPE <= TCP; TYPE++) {
            memset(&server_addr[TYPE], 0, sizeof(server_addr[TYPE]));
            server_addr[TYPE].sin_family = AF_INET;
            server_addr[TYPE].sin_port = htons(ports[TYPE]);
            server_addr[TYPE].sin_addr.s_addr = convertedIp;
        }

        memset(&msg, 0, sizeof(msg));

        // sync flags initially set to false
        server_quit = false;
        quit_listener = false;
        quit = false;
    }
    Client::~Client() {}

    /**
     * Estabilishes the connection with the server.
     */
    int Client::Connect() {
        spdlog::info("Trying to connect to the server...");

        /***************** TCP *****************/

        // connect to server
        sockets[TCP] = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sockets[TCP], (struct sockaddr*)&server_addr[TCP], sizeof(server_addr[TCP])) == -1) {
            spdlog::error("Failed to stablish a TCP connection. The server may be disconnected.");
            if (errno) perror("");
            return 1;
        }

        // the server responds with the player number
        char incoming_msg[MAXLINE];
        int n = recv(sockets[TCP], incoming_msg, MAXLINE, 0);
        incoming_msg[n] = '\0';
        spdlog::trace("Received from server using TCP {}", incoming_msg);
        player_num = atoi(incoming_msg);

        /***************** UDP *****************/

        // UDP socket
        sockets[UDP] = socket(AF_INET, SOCK_DGRAM, 0);
        char buffer[MAXLINE];
        sprintf(buffer, "I AM %d", player_num);

        // makes the socket nonblocking
        int dontblock = 1;
        auto rc = ioctl(sockets[UDP], FIONBIO, (char *) &dontblock);    

        // the server must save the client's UDP socket information
        // so the client sends a message informing who he is: "I am <player number>"
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

    /**
     * Creates the threads responsible for listening to the TCP and UDP connections.
     */
    void Client::StartListening() {
        thread_listen[UDP] = std::thread(&Client::ListenUDP, this);
        thread_listen[UDP].detach();

        thread_listen[TCP] = std::thread(&Client::ListenTCP, this);
        thread_listen[TCP].detach();
    }

    /**
     * Listens to the messages sent by the server through the UDP connection (game configuration messages);
     */
    void Client::ListenUDP() {
        static bool gameStarted = false;

        // used to keep track of how long since the last server contact
        using namespace std::chrono;
        high_resolution_clock::time_point last_contact;

        while (!quit_listener) {

            // if the server connection timed out, quit
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
            
            // since it is a nonblocking socket we must check if there was a message or not
            if( n == -1 ) 
                continue;
            
            // the game starts after the server sends the first game configuration message
            if (!gameStarted) {
                spdlog::info("Game started! Good luck and have fun!");
                gameStarted = true;
            }

            // update last contact time 
            last_contact = high_resolution_clock::now();

            // convert the message to host byte order
            msg.Deserialize();
            spdlog::trace("Received from server using UDP: {} {} {} {} {} {} {} {}", msg.xPlayer1, msg.yPlayer1, msg.xPlayer2, msg.yPlayer2, msg.xBall, msg.yBall, msg.scorePlayer1, msg.scorePlayer2);
        }
    }

    /**
     * Listens to the messages sent by the server through the TCP connection (game end messages);
     */
    void Client::ListenTCP() {

        // nonblocking TCP socket
        fcntl(sockets[TCP], F_SETFL, fcntl(sockets[TCP], F_GETFL) | O_NONBLOCK);

        while (!quit_listener) {
            char buffer[MAXLINE];
            int n = recv(sockets[TCP], buffer, MAXLINE, 0);
            if (n == -1) continue;
            buffer[n] = '\0';
            spdlog::trace("Received from server using TCP: {}", buffer);

            // if an EXIT message was received, quit
            if (strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0) {
                spdlog::info("Server disconnected!");

                // sets the flags used to synchronize the shutdown
                quit = true;
                server_quit = true;
                quit_listener = true;
            }
        }
    }

    /**
     * Sends the user input to the server through the UDP connection.
     */
    int Client::SendKeys() {
        // build string
        char client_msg[MAXLINE];
        sprintf(client_msg, "KEYS %d %d %d", player_num, keys[UP], keys[DOWN]);

        // send
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

    /**
     * Sets the key.
     */
    void Client::SetKey(int key, bool held) {
        keys[key] = held;
    }

    /**
     * Sends a message to the server announcing the end of the game. This method is called when self
     * quits the game.
     */
    int Client::AnnounceEnd() {
        // build string
        char client_msg[MAXLINE];
        strcpy(client_msg, USER_DESTROY);
        spdlog::trace("Asking server to exit with TCP: {}", client_msg);
        
        // send EXIT message
        if (send(sockets[TCP], (const char*)client_msg, strlen(client_msg), 0) < 0) {
            spdlog::error("Failed to send quit msg to the server");
            if (errno) perror("");
            return 1;
        }
        spdlog::trace("Successfully asked server to quit using TCP");

        return 0;
    }

    /**
     * Getters.
     */
    bool Client::GetQuit() { return quit; }
    bool Client::GetServerQuit() { return server_quit; }

    /**
     * Sets the flag used to quit the threads responsible for listening to the connections.
     */
    void Client::QuitListener() {
        quit_listener = true;
        spdlog::info("Stopped listening to the server!");
    }
}  // namespace Pong::Network::Client
#endif  // CLIENT