#if SERVER
#include "Server.h"

namespace Pong::Network::Server {
	Server::Server(const std::string& ServerIp, const std::string& ServerPort) {

		if(!ServerPort.empty() && ServerPort.find_first_not_of("0123456789") == std::string::npos) { // if port is a number
			port = std::stoi(ServerPort);
			if(port <= 0 || port > 65535) {
				std::cout << "[Error]: Port must be in range (0, 65535]!" << std::endl;
				exit(FAIL);
			}
		}
		else {
			std::cout << "[Error]: Port must be a number!" << std::endl;
			exit(FAIL);
		}
		
		ip = ServerIp;
		in_addr_t convertedIp;
		auto errCode = inet_pton(AF_INET, ip.c_str(), (void*) &convertedIp);
		if(errCode == 0) {
			std::cout << "[Error]: Invalid IP address!" << std::endl;
			exit(FAIL);
		}
		else if(errCode == -1) {
			std::cout << "inet_pton error!" << std::endl;
			exit(FAIL);
		}

		// Allocate a UDP socket for the server
		if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("Socket creation failure.");
			exit(FAIL);
		}

		// Assign a port and IP address to the server socket
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = convertedIp;
		memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

		// Bind socket to port
		if (bind(udp_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			perror("Bind failed.");
			exit(FAIL);
		}

		// Until a user calls destroy, server -> client communications will be of game configuration type
		strcpy( msg.type, OUTGOING_POSITION );  

		client_quit = false;
		quit_listener = false;
		quit = false;
	}
	Server::~Server() {}

	int Server::AcceptClient(int client_num) {
		// Accepts a client and saves its address for future reference
		char buffer[MAXLINE];
		unsigned len = sizeof(&(clients[client_num]));
		int n = recvfrom(udp_socket, (char*)buffer, MAXLINE, MSG_WAITALL,
			(struct sockaddr*)&(clients[client_num]), &len);
		buffer[n] = '\0';

		if (strcmp(buffer, NEW_CLIENT))
			return 1;

		// Says which player the client is
		const char* client_msg = std::to_string(client_num).c_str();

		sendto(udp_socket, client_msg, strlen(client_msg), MSG_CONFIRM,
			(const struct sockaddr*)&(clients[client_num]), len);
		return 0;
	}

	void Server::StartListeningUDP() {
		udp_thread_listen = std::thread(&Server::ListenUDP, this);
		udp_thread_listen.detach();
	}

	void Server::ListenUDP() {
		struct sockaddr_in client_addr;

		while ( !quit_listener ) {
			char buffer[MAXLINE];
			unsigned int len = sizeof(client_addr);
			memset(&client_addr, 0, sizeof(client_addr));
			int n = recvfrom(udp_socket, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr, &len);

			buffer[n] = '\0';
			//printf("%s\n", buffer);

			if (strncmp(INCOMING_KEYS, buffer, strlen(INCOMING_KEYS)) == 0) {
				char type[5];
				int isPlayerOne;
				int up;
				int down;

				sscanf(buffer, "%s %d %d %d", type, &isPlayerOne, &up, &down);

				if (up | down) {
					printf("UP: %d - DOWN: %d\n", up, down);
				}

				if (isPlayerOne) {
					keys[W] = up;
					keys[S] = down;
				}
				else {
					keys[UP] = up;
					keys[DOWN] = down;
				}
			}
			else if( strncmp(USER_DESTROY, buffer, strlen(USER_DESTROY)) == 0 ){
				quit = true;
				client_quit = true;
				quit_listener = true;
			}
		}
	}

	int Server::SendPosition(int client_num) {
		unsigned int len = sizeof(clients[client_num]);
		msg.Serialize();
		if (sendto(udp_socket, &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*)&clients[client_num], len) < 0) {
			return 1;
		}
		msg.Deserialize();

		return 0;
	}

	bool Server::GetKey(int key_num) {
		return keys[key_num];
	}

	int Server::AnnounceEnd( int client_num ){
		
		// wait for acknowledgement with a loop?

		// VERIFY whether there are clients already or not

		strcpy(msg.type, USER_DESTROY);

		unsigned int len = sizeof(clients[client_num]);
		msg.Serialize();
		if (sendto(udp_socket, &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*)&clients[client_num], len) < 0) {
			perror("Error sending quit message\n");
			return 1;
		}
		msg.Deserialize();

		return 0;
	}

	bool Server::GetQuit(){ return quit; }
	bool Server::GetClientQuit(){ return client_quit; }
	void Server::QuitListener(){ quit_listener = true; }
}
#endif