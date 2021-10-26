#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace Pong::Server {

    constexpr int SERVER_PORT = 1234;

    class Server {

    private:
        int my_socket;  // server socket

    public:
        Server();
        AcceptClient();
    };
}
#endif  // SERVER_H