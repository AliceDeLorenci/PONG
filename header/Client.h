#if CLIENT
#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "Network.h"
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

namespace Pong::Network::Client {

	static constexpr const char* 	CONNECT_HANDSHAKE 	= "WHO AM I";
	static constexpr const char* 	INCOMING_POSTION 	= "CONF";
	static constexpr const char* 	USER_DESTROY 		= "EXIT";

	enum KeyPlayer { UP, DOWN };
	enum Connections { UDP, TCP };

	class Client {
	private:
		std::array<std::thread, 2> thread_listen;       // thread_listen[CONNECTION TYPE] thread responsible for listening to server
		std::array<struct sockaddr_in, 2> server_addr;	// server_addr[CONNECTION TYPE]
		std::array<int, 2> ports;						// ports[CONNECTION TYPE]
		std::array<int, 2> sockets;						// sockets[CONNECTION TYPE]

		int player_num;									// Which player am I
		std::array<bool, 2> keys;                       // {UP, DOWN}, 1 = held

		bool server_quit;								// quit flag, set when an outsider starts the quitting process
		bool quit_listener;								// used to quit the listener thread			
		bool quit;										// general quit flag

	public:
		Client(const std::string& ServerIp = LOCALHOST, const std::string& UDPServerPort = DEFAULT_UDP_PORT, const std::string& TCPServerPort = DEFAULT_TCP_PORT);
		virtual ~Client();

		int Connect();									// Attempts to create a connection with the server

		void StartListening();      					// Starts thread running listening to UDP and TCP
		void ListenUDP();             					// Listen for server input
		void ListenTCP();        						// Listen for server exiting

		int SendKeys();									// Send currently pressed keys to server
		void SetKey(int, bool);							// Sets a key in the keys array

		bool GetServerQuit();
		bool GetQuit();

		void QuitListener();							// Sets the quit_listener flag
		int AnnounceEnd();								// Tell the server you're quitting

		void RuntimeMessage( std::string );				// Prints runtime message on terminal

		Pong::Network::GameInfo::GameInfo msg;          // Message received from server
	};
}
#endif  // CLIENT_H
#endif  // CLIENT