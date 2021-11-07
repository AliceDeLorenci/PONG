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

//#define INCOMING_CLIENT "WHO AM I"

namespace Pong::Network::Server {
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
        std::array<bool, 4> keys = {0, 0, 0, 0};  // 1 = held

        std::array<std::thread, 2> thread_listen;       // thread_listen[CONNECTION TYPE] thread responsible for listening to clients
        std::array<struct sockaddr_in, 2> UDP_clients;  // UDP_clients[ClientNum]
        std::array<int, 2> TCP_clients = {-1, -1};      // TCP_clients[ClientNum]
        std::array<int, 2> ports;                       // ports[CONNECTION TYPE]
        std::array<int, 2> sockets;                     // sockets[CONNECTION TYPE]
        std::string ip;                                 // Server IP
        in_addr_t convertedIp;                          // Server IP converted to in_addr_t

        bool client_quit;    // quit flag, set when a client starts the quitting process
        bool quit_listener;  // used to quit the listener thread
        bool quit;           // general quit flag

     public:
        Server(const std::string& ServerIp = LOCALHOST, const std::string& UDPServerPort = DEFAULT_UDP_PORT, const std::string& TCPServerPort = DEFAULT_TCP_PORT);
        virtual ~Server();

        int CreateConnection(int, const std::string&);

        int AcceptClient(int);  // Clients join game

        void StartListeningUDP();     // Starts thread running listening to UDP
        void ListenUDP();             // Listen for client input
        void ListenTCP();             // Listen for client exiting
        bool IsClientConnected(int);  // Informs if a given client is connected or not

        int SendPosition(int);  // Sends position to clients
        bool GetKey(int);       // Receive client key

        bool GetQuit();
        bool GetClientQuit();

        int AnnounceEnd(int);  // Order every client to quit
        void QuitListener();   // Sets the quit_listener flag

        std::string GetClientsIp();

        GameInfo::GameInfo msg;  // Message sent to client
    };
}  // namespace Pong::Network::Server
#endif  // SERVER_H
#endif  // SERVER