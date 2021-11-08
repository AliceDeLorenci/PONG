#if CLIENT
#pragma once
#ifndef CLIENT_H
#define CLIENT_H

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
#include <thread>

#include "Network.h"

/**
 * The gamers are clients. 
 * The client program only reads the user input, sends it to the server 
 * and receives from the server the game configuration.
 * The server is responsible for all the game processing.
 */
namespace Pong::Network::Client {

    // expressions used on the different messages sent and received over the network
    // to identify the different types of messages
    static constexpr const char* CONNECT_HANDSHAKE = "WHO AM I";
    static constexpr const char* INCOMING_POSTION = "CONF";
    static constexpr const char* USER_DESTROY = "EXIT";

    enum KeyPlayer { UP,
                     DOWN };
    enum Connections { UDP,
                       TCP };

    class Client {
     private:

        // network connection related variables
        std::array<std::thread, 2> thread_listen;       // thread_listen[CONNECTION TYPE] thread responsible for listening to server
        std::array<struct sockaddr_in, 2> server_addr;  // server_addr[CONNECTION TYPE]
        std::array<int, 2> ports;                       // ports[CONNECTION TYPE]
        std::array<int, 2> sockets;                     // sockets[CONNECTION TYPE]

        int player_num;                                 // which player I am 
        std::array<bool, 2> keys;                       // {UP, DOWN}, 1 = held

        // flags used to synchronize game shutdown
        bool server_quit;    // quit flag, set when an outsider starts the quitting process
        bool quit_listener;  // used to quit the listener thread
        bool quit;           // general quit flag

     public:
        Client(const std::string& ServerIp = LOCALHOST, const std::string& UDPServerPort = DEFAULT_UDP_PORT, const std::string& TCPServerPort = DEFAULT_TCP_PORT);
        virtual ~Client();

        // connection setup
        int Connect();                      // Attempts to create a connection with the server

        // listening to the connections
        void StartListening();              // Starts thread running listening to UDP and TCP
        void ListenUDP();                   // Listen for server input
        void ListenTCP();                   // Listen for server exiting

        // user input
        int SendKeys();                     // Send currently pressed keys to server
        void SetKey(int, bool);             // Sets a key in the keys array

        // quitting the game
        int AnnounceEnd();                  // Tell the server you're quitting
        void QuitListener();                // Sets the quit_listener flag
        bool GetServerQuit();
        bool GetQuit();

        // Message received from the server (game configuration)
        Pong::Network::GameInfo::GameInfo msg;  
    };
}  // namespace Pong::Network::Client
#endif  // CLIENT_H
#endif  // CLIENT