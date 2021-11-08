#if SERVER
#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>
#include <thread>

#include "Network.h"

/**
 * The server is responsible for all the game processing. 
 * It receives from the clients the user inputs and sends to them the game configuration.
 */

namespace Pong::Network::Server {

    // expressions used on the different messages sent and received over the network
    // to identify the different types of messages
    static constexpr const char* NEW_TCP_CLIENT = "WHO AM I";
    static constexpr const char* NEW_UDP_CLIENT = "I AM";
    static constexpr const char* CONFIRM_UDP_CLIENT = "I AM";
    static constexpr const char* INCOMING_KEYS = "KEYS";
    static constexpr const char* OUTGOING_POSITION = "CONF";
    static constexpr const char* USER_DESTROY = "EXIT";

    enum ClientNum { ClientOne,
                     ClientTwo };
    enum KeyPlayer { W,
                     S,
                     UP,
                     DOWN };

    class Server {
     private:
        // user input
        std::array<bool, 4> keys = {0, 0, 0, 0};  // 1 = held

        // network connection related variables
        std::array<std::thread, 2> thread_listen;       // thread_listen[CONNECTION TYPE] thread responsible for listening to clients
        std::array<struct sockaddr_in, 2> UDP_clients;  // UDP_clients[ClientNum]
        std::array<int, 2> TCP_clients = {-1, -1};      // TCP_clients[ClientNum]
        std::array<int, 2> ports;                       // ports[CONNECTION TYPE]
        std::array<int, 2> sockets;                     // sockets[CONNECTION TYPE]
        std::string ip;                                 // Server IP
        in_addr_t convertedIp;                          // Server IP converted to in_addr_t

        // flags used to synchronize game shutdown
        bool client_quit;                               // quit flag, set when a client starts the quitting process
        bool quit_listener;                             // used to quit the listener thread
        bool quit;                                      // general quit flag

     public:
        Server(const std::string& ServerIp = LOCALHOST, const std::string& UDPServerPort = DEFAULT_UDP_PORT, const std::string& TCPServerPort = DEFAULT_TCP_PORT);
        virtual ~Server();

        // connection setup
        int CreateConnection(int, const std::string&);
        int AcceptClient(int);          // Clients join game
        std::string GetClientsIp();

        // listening to the connections
        void StartListeningUDP();       // Starts thread running listening to UDP
        void ListenUDP();               // Listen for client input
        void ListenTCP();               // Listen for client exiting
        bool IsClientConnected(int);    // Informs if a given client is connected or not
        
        // sending and receiving game control messages
        int SendPosition(int);          // Sends position to clients
        bool GetKey(int);               // Receive client key

        // quitting the game
        int AnnounceEnd(int);           // Order every client to quit
        void QuitListener();            // Sets the quit_listener flag
        bool GetQuit();
        bool GetClientQuit();

        // Message sent to client (game configuration)
        GameInfo::GameInfo msg;  
    };
}  // namespace Pong::Network::Server
#endif  // SERVER_H
#endif  // SERVER