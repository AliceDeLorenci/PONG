#if SERVER
#include "Server.h"

#include "spdlog/spdlog.h"

/**
 * The server does all the game processing. It receives from the clients the user inputs 
 * and sends to them the game configuration.
 */
namespace Pong::Network::Server {
    /**
     * Constructs a new Server::Server and configures its sockets (UDP and TCP).
     */
    Server::Server(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort)
        : ip(ServerIp), TCP_clients({-1, -1}), client_quit(false), quit_listener(false), quit(false) {
        CreateConnection(UDP, UDPServerPort);
        CreateConnection(TCP, TCPServerPort);
    }
    Server::~Server() {}

    /**
     * Configure the server sockets.
     */
    int Server::CreateConnection(int TYPE, const std::string& ServerPort) {
        ports[TYPE] = ConvertPort(ServerPort);

        int socket_type = (TYPE == UDP) ? SOCK_DGRAM : SOCK_STREAM;

        // allocate a socket for the server
        if ((sockets[TYPE] = socket(AF_INET, socket_type, 0)) < 0) {
            spdlog::error("Failed to create a server socket!");
            if (errno) perror("");
            exit(EXIT_FAILURE);
        }

        // attempts to stop the port from blocking after repeated executions
        int allow = 1;
        if (setsockopt(sockets[TYPE], SOL_SOCKET, SO_REUSEADDR, &allow, sizeof(int)) < 0)  // Allows reuse of local address
            spdlog::error("Failed to allow the reuse of local address for server socket of type {}", TYPE);
            
        // assign a port and IP address to the server socket
        auto convertedIp = ConvertIp(ip);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ports[TYPE]);
        addr.sin_addr.s_addr = convertedIp;
        memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

        // bind socket to port
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

    /**
     * Establishes the connection with the clients.
     */
    int Server::AcceptClient(int clientNum) {
        
        /***************** TCP *****************/

        // accepts a TCP connection
        if ((TCP_clients[clientNum] = accept(sockets[TCP], 0, 0)) < 0) {
            spdlog::error("Failed to accept client!");
            if (errno) perror("");
            return 1;
        }

        // informs which player the client is
        const char* client_msg = std::to_string(clientNum).c_str();
        send(TCP_clients[clientNum], client_msg, strlen(client_msg), 0);
        spdlog::trace("Sent to client {} using TCP: {}", clientNum, client_msg);

        /***************** UDP *****************/

        // accepts a UDP client and saves its address for future reference
        // the client inform whom he is: "I AM <player number>"

        int dontblock = 1;
        auto rc = ioctl(sockets[UDP], FIONBIO, (char *) &dontblock);    // nonblocking UDP socket
        
        // keep track of how long the server is wainting for a contact
        using namespace std::chrono;
        high_resolution_clock::time_point first_attempt = high_resolution_clock::now(); 
        while (true) {
            
            // connection timeout
            if( duration_cast<milliseconds>(high_resolution_clock::now() - first_attempt).count() > TIMEOUT ){
                    spdlog::info("Player {} connection timed out", clientNum + 1);
                    quit_listener = true;
                    quit = true;
                    return 1;
            }

            char buffer[MAXLINE];
            unsigned len = sizeof(&(UDP_clients[clientNum]));
            int n = recvfrom(sockets[UDP], (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&UDP_clients[clientNum], &len);
            if( n == -1 )
                continue;

            buffer[n] = '\0';
            spdlog::trace("Received from a client using UDP: {}", buffer);

            if (!strncmp(buffer, NEW_UDP_CLIENT, strlen(NEW_UDP_CLIENT)))   // the message was not as expected
                continue;

            if (atoi(&buffer[strlen(NEW_UDP_CLIENT) + 1]) != clientNum)     // player number inconsistent with what was expected
                continue;

            break;
        }

        // makes the TCP socket non-blocking
        fcntl(TCP_clients[clientNum], F_SETFL, fcntl(TCP_clients[clientNum], F_GETFL) | O_NONBLOCK);

        // start listening to the client through the TCP connection (quit messages)
        if (clientNum == ClientOne) {
            thread_listen[TCP] = std::thread(&Server::ListenTCP, this);
            thread_listen[TCP].detach();
        }

        return 0;
    }

    /**
     * Initializes the thread that will listen to the UDP connection.
     */
    void Server::StartListeningUDP() {
        thread_listen[UDP] = std::thread(&Server::ListenUDP, this);
        thread_listen[UDP].detach();
    }

    /**
     * Listens to the UDP connection (user input messages).
     */
    void Server::ListenUDP() {
        struct sockaddr_in client_addr;

        // keep track of the time of the last contact in order to identify timeout
        using namespace std::chrono;
        std::array<high_resolution_clock::time_point, 2> last_contact = { high_resolution_clock::now(), high_resolution_clock::now() };

        while (!quit_listener) {

            // connection timeout
            for( int client = ClientOne; client <= ClientTwo; client ++ )
                if( duration_cast<milliseconds>(high_resolution_clock::now() - last_contact[client]).count() > TIMEOUT ){
                    spdlog::info("Player {} connection timed out", client + 1);
                    quit_listener = true;
                    quit = true;
                    return;
                }
            
            // checks whether there is a new message
            char buffer[MAXLINE];
            socklen_t len = sizeof(client_addr);
            memset(&client_addr, 0, sizeof(client_addr));
            int n = recvfrom(sockets[UDP], (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr, &len);
            if( n == -1 )
                continue;

            buffer[n] = '\0';
            spdlog::trace("Received from a client using UDP: {}", buffer);

            // if the message has the correct structure
            if (strncmp(INCOMING_KEYS, buffer, strlen(INCOMING_KEYS)) == 0) {
                char type[5];
                int isPlayerTwo;
                int up;
                int down;

                sscanf(buffer, "%s %d %d %d", type, &isPlayerTwo, &up, &down);

                // save user input
                if (!isPlayerTwo) {
                    keys[W] = up;
                    keys[S] = down;
                    last_contact[ClientOne] = high_resolution_clock::now();
                } else {
                    keys[UP] = up;
                    keys[DOWN] = down;
                    last_contact[ClientTwo] = high_resolution_clock::now();
                }
            }
        }
    }

    /**
     * Listens to the TCP connection (quit messages).
     */
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

                // received an EXIT message
                if (strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0) {
                    spdlog::info("Player {} disconnected", client + 1);

                    // client disconnected
                    TCP_clients[client] = -1;

                    // sets the flags used to synchronize the shutdown
                    quit = true;
                    client_quit = true;
                    quit_listener = true;
                    break;
                }
            }
        }
    }

    /**
     * Sends the game configuration to the client.
     */
    int Server::SendPosition(int client_num) {
        socklen_t len = sizeof(UDP_clients[client_num]);
        spdlog::trace("Sending to client {} using TCP: {} {} {} {} {} {} {} {}", client_num, msg.xPlayer1, msg.yPlayer1, msg.xPlayer2, msg.yPlayer2, msg.xBall, msg.yBall, msg.scorePlayer1, msg.scorePlayer2);
       
        // message in network byte order
        msg.Serialize();
        if (sendto(sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*)&UDP_clients[client_num], len) < 0) {
            spdlog::error("Could not send position to client {}!", client_num);
            return 1;
        }
        // struct converted back to host byte order
        msg.Deserialize();
        spdlog::trace("Sent position to client {}", client_num);

        return 0;
    }

    /**
     * Announces to the client that the game has ended.
     */
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

    /**
     * Getters.
     */
    bool Server::GetKey(int key_num) { return keys[key_num]; }
    bool Server::GetQuit() { return quit; }
    bool Server::GetClientQuit() { return client_quit; }

    /**
     * Checks whether a specific client is connected.
     */
    bool Server::IsClientConnected(int client) { return !(TCP_clients[client] < 0); }

    /**
     * Sets the flag used to quit the threads responsible for listening to the connections.
     */
    void Server::QuitListener() {
        quit_listener = true;
        spdlog::info("Stopped receiving players commands!");
    }

    /**
     * Get the clients' IP addresses.
     */
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