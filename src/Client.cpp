#if CLIENT
#include "Client.h"

#include <errno.h>

namespace Pong::Network::Client {
	Client::Client(const std::string& ServerIp, const std::string& ServerPort) {
		
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

		// Allocate a UDP socket for the client
		if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("Socket creation failure.");
			exit(FAIL);
		}

		// Server information
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = convertedIp;

		memset(&msg, 0, sizeof(msg));

		server_quit = false;
		quit_listener = false;
		quit= false;
	}
	Client::~Client() {}

	int Client::Connect() {
		char buffer[MAXLINE];

		unsigned int len = sizeof(server_addr);
		sendto(udp_socket, (const char*)CONNECT_HANDSHAKE, strlen(CONNECT_HANDSHAKE), MSG_CONFIRM,
			(const struct sockaddr*)&server_addr, len);

		// Receives player number
		len = sizeof(server_addr);
		int n = recvfrom(udp_socket, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&server_addr, &len);

		buffer[n] = '\0';
		player_num = atoi(buffer);

		return 0;
	}

	void Client::StartListeningUDP() {
		thread_listen = std::thread(&Client::ListenUDP, this);
		thread_listen.detach();
	}

	void Client::ListenUDP() {
		while ( !quit_listener ) {
			unsigned int len = sizeof(server_addr);
			recvfrom(udp_socket, &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_WAITALL, (struct sockaddr*)&server_addr, &len);
			msg.Deserialize();

			if( strcmp( msg.type, USER_DESTROY ) ==  0 ){
				quit = true;
				server_quit = true;
				quit_listener = true;
			}
		}
	}

	int Client::SendKeys() {
		errno = 0;

		// build string
		char client_msg[MAXLINE];
		sprintf(client_msg, "KEYS %d %d %d", player_num, keys[UP], keys[DOWN]);

		/*
		if (keys[UP] || keys[DOWN]) {
			printf("UP: %d - DOWN: %d\n", keys[UP], keys[DOWN]);
		}
		*/

		// Sends game configuration
		unsigned int len = sizeof(server_addr);
		if (sendto(udp_socket, (const char*)client_msg, strlen(client_msg), MSG_CONFIRM,
			(const struct sockaddr*)&server_addr, len) < 0) {
			perror("Error sending msg\n");
			return 1;
		}
		return 0;
	}

	void Client::SetKey(int key, bool held) {
		keys[key] = held;
	}

	int Client::AnnounceEnd( ){
		
		// wait for acknowledgement with a loop?

		// build string
		char client_msg[MAXLINE];
		strcpy( client_msg, USER_DESTROY );

		unsigned int len = sizeof( server_addr );
		if (sendto(udp_socket, (const char*)client_msg, strlen(client_msg), MSG_CONFIRM,
			(const struct sockaddr*)&server_addr, len) < 0) {
			perror("Error sending quit msg\n");
			return 1;
		}

		return 0;
	}

	bool Client::GetQuit(){ return quit; }
	bool Client::GetServerQuit(){ return server_quit; }
	void Client::QuitListener(){ quit_listener = true; }
}
#endif // CLIENT