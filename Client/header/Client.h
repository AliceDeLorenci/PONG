#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <thread>
#include <array>

#define EXIT_FAILURE 1

namespace Client {

    constexpr int SERVER_PORT = 1234;
    constexpr int MAXLINE = 1024;

    static constexpr const char* CONNECT_HANDSHAKE = "WHO AM I";
    static constexpr std::string_view INCOMING_POSTION = "CONF";

    enum KeyPlayer { UP, DOWN };

    class Client {

    private:
        std::thread thread_listen;                      // thread responsible for listening to server
        int my_socket;
        struct sockaddr_in server_addr;
        int player_num;
        std::array<bool, 2> keys;                       // 1 = held

    public:
        Client();
        int Connect();
        void StartListening();
        void Listen();
        int SendKeys();
        void SetKey( int, bool );
       
        struct game_info{
            int xPlayer1; 
            int yPlayer1; 
            int xPlayer2; 
            int yPlayer2; 
            int xBall; 
            int yBall;
            int scorePlayer1; 
            int scorePlayer2;
        } msg;                  // Message received from server
    };
}
#endif  // CLIENT_H