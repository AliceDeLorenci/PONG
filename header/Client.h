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
	constexpr int SERVER_PORT = 1234;
	constexpr int MAXLINE = 1024;
	constexpr int FAIL = 1;

	static constexpr const char* CONNECT_HANDSHAKE = "WHO AM I";
	static constexpr std::string_view INCOMING_POSTION = "CONF";
	static constexpr const char* USER_DESTROY = "EXIT";

	enum KeyPlayer { UP, DOWN };
	enum Connections { UDP, TCP };

	class Client {
	private:
		std::thread udp_thread_listen;                  // thread responsible for listening to server (udp connection)
		std::thread tcp_thread_listen;                  // thread responsible for listening to server (tcp connection)

		std::array<struct sockaddr_in, 2> server_addr;	// server_addr[CONNECTION TYPE]
		int player_num;									// Which player am I
		std::array<bool, 2> keys;                       // {UP, DOWN}, 1 = held

		std::array<int, 2> ports;
		std::array<int, 2> sockets;

		bool server_quit;								// quit flag, set when an outsider starts the quitting process
		bool quit_listener;								// used to quit the listener thread			
		bool quit;										// general quit flag

	public:
		Client(const std::string& ServerIp = "127.0.0.1", const std::string& UDPServerPort = "1234", const std::string& TCPServerPort = "1235");
		virtual ~Client();

		int Connect();									// Attempts to create a connection with the server
		void StartListeningUDP();						// Starts the udp_thread_listen
		void ListenUDP();								// Listen for positions from server

		/*
		void StartListeningTCP();      					// Starts thread running Listen()
		void ListenTCP();             					// Listen for client input
		*/

		int SendKeys();									// Send currently pressed keys to server
		void SetKey(int, bool);							// Sets a key in the keys array

		bool GetServerQuit();
		bool GetQuit();

		void QuitListener();							// Sets the quit_listener flag
		int AnnounceEnd();								// Tell the server you're quitting

		Pong::Network::GameInfo::GameInfo msg;          // Message received from server
	};
}
#endif  // CLIENT_H
#endif  // CLIENT