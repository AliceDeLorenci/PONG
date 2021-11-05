#if SERVER
#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "Network.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
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
	static constexpr const char* NEW_TCP_CLIENT 	= "WHO AM I";
	static constexpr const char* NEW_UDP_CLIENT 	= "I AM";
	static constexpr const char* CONFIRM_UDP_CLIENT = "I AM";
	static constexpr const char* INCOMING_KEYS 		= "KEYS";
	static constexpr const char* OUTGOING_POSITION 	= "CONF";
	static constexpr const char* USER_DESTROY 		= "EXIT";

	enum ClientNum { ClientOne, ClientTwo };
	enum KeyPlayer { W, S, UP, DOWN };
	enum Connections { UDP, TCP };

	class Server {
	private:
		std::array<std::thread, 2> thread_listen;       // thread_listen[CONNECTION TYPE] thread responsible for listening to clients
		std::thread temporary_listener;					// while only one client is connected, listen to quitting messages from him
		std::array<struct sockaddr_in, 2> UDP_clients;  // UDP_clients[ClientNum]
		std::array<int, 2> TCP_clients;					// TCP_clients[ClientNum]
		std::array<bool, 4> keys = { 0,0,0,0 };         // 1 = held
		
		std::array<int, 2> ports;						// ports[CONNECTION TYPE]
		std::array<int, 2> sockets;						// sockets[CONNECTION TYPE]
		
		std::string ip;									// Server IP
		in_addr_t convertedIp;							// Server IP converted to in_addr_t
		
		bool connection_success;						// used to quit the temporary listener

		bool client_quit;  								// quit flag, set when a client starts the quitting process
		bool quit_listener;								// used to quit the listener thread
		bool quit;		 								// general quit flag

	public:
		Server(const std::string& ServerIp = LOCALHOST, const std::string& UDPServerPort = DEFAULT_UDP_PORT, const std::string& TCPServerPort = DEFAULT_TCP_PORT);
		virtual ~Server();

		int CreateConnection( int, const std::string& );

		int AcceptClient(int);    						// Clients join game

		void StartListening();      					// Starts thread running listening to UDP and TCP
		void ListenUDP();             					// Listen for client input
		void ListenTCP();        						// Listen for client exiting
		void ListenToOneClient(int);					// Listen to the only client connected
		bool IsClientConnected(int);					// Informs if a given client is connected or not

		int SendPosition(int);    						// Sends position to clients
		bool GetKey(int);								// Receive client key

		bool GetQuit();
		bool GetClientQuit();
		
		int AnnounceEnd( int );							// Order every client to quit
		void QuitListener();							// Sets the quit_listener flag

		void RuntimeMessage( std::string );				// Prints runtime message on terminal

		GameInfo::GameInfo msg; 			// Message sent to client
	};
}
#endif  // SERVER_H
#endif  // SERVER