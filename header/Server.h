#if SERVER
#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "GameInfo.h"
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
#include <thread>
#include <string>

//#define INCOMING_CLIENT "WHO AM I"

namespace Pong::Network::Server {
	constexpr int SERVER_PORT = 1234;
	constexpr int MAXLINE = 1024;
	constexpr int FAIL = 1;

	static constexpr const char* NEW_CLIENT = "WHO AM I";
	//static constexpr std::string_view INCOMING_KEYS = "KEYS";
	static constexpr const char* INCOMING_KEYS = "KEYS";

	enum ClientNum { ClientOne, ClientTwo };
	enum KeyPlayer { W, S, UP, DOWN };

	class Server {
	private:
		int my_socket;  // server socket
		std::thread thread_listen;                      // thread responsible for listening to clients
		std::array<std::thread, 2> thread_talk;         // threads responsible for talking to clients
		std::array<struct sockaddr_in, 2> clients;      // client addresses
		std::array<bool, 4> keys = { 0,0,0,0 };           // 1 = held

	public:
		Server();
		int AcceptClient(int);    // Clients join game
		void StartListening();      // Starts thread running Listen()
		void Listen();             // Listen for client input
		int SendPosition(int);    // Sends position to clients
		bool GetKey(int);

		Pong::Network::GameInfo::GameInfo msg; // Message sent to client
	};
}
#endif  // SERVER_H
#endif  // SERVER