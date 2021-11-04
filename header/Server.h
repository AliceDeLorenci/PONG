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
	static constexpr const char* OUTGOING_POSITION = "CONF";
	static constexpr const char* USER_DESTROY = "EXIT";

	enum ClientNum { ClientOne, ClientTwo };
	enum KeyPlayer { W, S, UP, DOWN };

	class Server {
	private:
		int udp_socket;  								// socket for communicating game information
		int tcp_socket;									// socket for communicating server information
		std::thread udp_thread_listen;                  // thread responsible for listening to clients (udp connection)
		std::thread tcp_thread_listen;                  // thread responsible for listening to clients (tcp connection)
		std::array<struct sockaddr_in, 2> clients;      // client addresses
		std::array<bool, 4> keys = { 0,0,0,0 };         // 1 = held
		
		std::string ip;
		int port;
		
		bool client_quit;  								// quit flag, set when a client starts the quitting process
		bool quit_listener;								// used to quit the listener thread
		bool quit;		 								// general quit flag

	public:
		Server(const std::string& ServerIp = "127.0.0.1", const std::string& ServerPort = "1234");
		virtual ~Server();

		int CreateUDPConnection();
		int CreateTCPConnection();

		int AcceptClient(int);    						// Clients join game

		void StartListeningUDP();      					// Starts thread running Listen()
		void ListenUDP();             					// Listen for client input
		int SendPosition(int);    						// Sends position to clients
		bool GetKey(int);								// Receive client key

		bool GetQuit();
		bool GetClientQuit();
		
		int AnnounceEnd( int );							// Warns every client to quit
		void QuitListener();							// Sets the quit_listener flag

		Pong::Network::GameInfo::GameInfo msg; 			// Message sent to client
	};
}
#endif  // SERVER_H
#endif  // SERVER