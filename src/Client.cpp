#if CLIENT
#include "Client.h"

#include <errno.h>

namespace Pong::Network::Client {
	Client::Client(const std::string& ServerIp, const std::string& ServerPort) {
		ip = ServerIp;
		port = ServerPort;
		// Allocate a UDP socket for the client
		if ((my_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("Socket creation failure.");
			exit(FAIL);
		}

		// Server information
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		memset(&msg, 0, sizeof(msg));

		quit = false;
	}
	Client::~Client() {}

	int Client::Connect() {
		char buffer[MAXLINE];

		unsigned int len = sizeof(server_addr);
		sendto(my_socket, (const char*)CONNECT_HANDSHAKE, strlen(CONNECT_HANDSHAKE), MSG_CONFIRM,
			(const struct sockaddr*)&server_addr, len);

		// Receives player number
		len = sizeof(server_addr);
		int n = recvfrom(my_socket, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&server_addr, &len);

		buffer[n] = '\0';
		player_num = atoi(buffer);

		return 0;
	}

	void Client::StartListening() {
		thread_listen = std::thread(&Client::Listen, this);
	}

	void Client::Listen() {
		while ( !quit ) {
			unsigned int len = sizeof(server_addr);
			recvfrom(my_socket, &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_WAITALL, (struct sockaddr*)&server_addr, &len);
			msg.Deserialize();

			if( strcmp( msg.type, USER_DESTROY ) ==  0 ){
				quit = true;
			}
		}
	}

	int Client::SendKeys() {
		errno = 0;

		// montar string
		char client_msg[MAXLINE];
		sprintf(client_msg, "KEYS %d %d %d", player_num, keys[UP], keys[DOWN]);

		if (keys[UP] || keys[DOWN]) {
			printf("UP: %d - DOWN: %d\n", keys[UP], keys[DOWN]);
		}

		// Sends game configuration
		unsigned int len = sizeof(server_addr);
		if (sendto(my_socket, (const char*)client_msg, strlen(client_msg), MSG_CONFIRM,
			(const struct sockaddr*)&server_addr, len) < 0) {
			perror("Error sending msg\n");
			return 1;
		}
		return 0;
	}

	void Client::SetKey(int key, bool held) {
		keys[key] = held;
	}

	bool Client::GetQuit() { return quit; }
}
#endif // CLIENT