#if SERVER
#include "Server.h"

namespace Pong::Network::Server {
	Server::Server(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) {

		ip = ServerIp;
		CreateConnection( UDP, UDPServerPort );
		CreateConnection( TCP, TCPServerPort );

		TCP_clients = { -1, -1 };

		client_quit = false;
		quit_listener = false;
		quit = false;
	}
	Server::~Server() {}

	int Server::CreateConnection( int TYPE, const std::string& ServerPort ){

		ports[TYPE] = CheckPort( ServerPort );

		int socket_type;
		if( TYPE == UDP )
			socket_type = SOCK_DGRAM;
		else
			socket_type = SOCK_STREAM;

        // Allocate a socket for the server
        if( ( sockets[TYPE] = socket( AF_INET, socket_type, 0 ) ) < 0 ){
            perror("[Error] Failed to create a server socket!\n");
            exit(EXIT_FAILURE);
        }  

		// Assign a port and IP address to the server socket
		auto convertedIp = CheckIp( ip );
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons( ports[TYPE] );
		addr.sin_addr.s_addr = convertedIp;
		memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

		// Bind socket to port
		if (bind( sockets[TYPE], (struct sockaddr*) &addr, sizeof(addr)) < 0 ) {
			perror("[Error] Failed to bind a server socket!\n");
			exit(FAIL);
		}

		if( TYPE == TCP ){
			// socket in passive open mode
			if( listen( sockets[TCP], 1 ) == -1 ) {
				perror("[Error]: Could not prepare to accept connections on listen()\n");
    			return 1;
  			}
		}
		
		return 0;
	}

	int Server::AcceptClient( int client_num ) {

		// TCP
		// Server: "<player number>"

		// Accepts a TCP client and saves the corresponding socket number
		if( (TCP_clients[client_num] = accept( sockets[TCP], 0, 0 )) < 0 ){
			perror("[Error] failed to accept client!\n");
			return 1;
		}

		// Says which player the client is
		const char* client_msg = std::to_string(client_num).c_str();
		send( TCP_clients[client_num], client_msg, strlen(client_msg), 0 );

		// UDP
		// Client: "I am <player number>"

		// Accepts a UDP client and saves its address for future reference
		while( true ){
			char buffer[MAXLINE];
			unsigned len = sizeof( &(UDP_clients[client_num]) );
			int n = recvfrom( sockets[UDP], (char*) buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &UDP_clients[client_num], &len);
			buffer[n] = '\0';

			if ( !strncmp( buffer, NEW_UDP_CLIENT, strlen(NEW_UDP_CLIENT) ) )
				continue;
			
			if( atoi( &buffer[ strlen(NEW_UDP_CLIENT) + 1 ] ) != client_num )
				continue;

			break;
		}

		return 0;

	}

	void Server::StartListening() {
		thread_listen[UDP] = std::thread(&Server::ListenUDP, this);
		thread_listen[UDP].detach();

		thread_listen[TCP] = std::thread(&Server::ListenTCP, this);
		thread_listen[TCP].detach();
	}

	void Server::ListenUDP() {
		struct sockaddr_in client_addr;

		while ( !quit_listener ) {
			char buffer[MAXLINE];
			unsigned int len = sizeof(client_addr);
			memset(&client_addr, 0, sizeof(client_addr));
			int n = recvfrom(sockets[UDP], (char*) buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &client_addr, &len);

			buffer[n] = '\0';

			if (strncmp(INCOMING_KEYS, buffer, strlen(INCOMING_KEYS)) == 0) {
				char type[5];
				int isPlayerOne;
				int up;
				int down;

				sscanf(buffer, "%s %d %d %d", type, &isPlayerOne, &up, &down);

				if (isPlayerOne) {
					keys[W] = up;
					keys[S] = down;
				}
				else {
					keys[UP] = up;
					keys[DOWN] = down;
				}
			}
		}
	}

	void Server::ListenTCP( ) {
		// makes the socket non-blocking
		for(int client = ClientOne; client <= ClientTwo; client++)
			fcntl(TCP_clients[client], F_SETFL, fcntl(TCP_clients[client], F_GETFL) | O_NONBLOCK);

		while ( !quit_listener ) {
			for(int client = ClientOne; client <= ClientTwo; client++) {
				char buffer[MAXLINE];
				int n = recv( TCP_clients[client], buffer, MAXLINE, 0 );

				if(n == -1) continue;
				buffer[n] = '\0';

				if ( strncmp( USER_DESTROY, buffer, strlen(USER_DESTROY) ) == 0 ) {
					std::string status_msg;
					status_msg = "[STATUS] Client " + std::to_string(client) + " disconnected";
					RuntimeMessage( status_msg );

					quit = true;
					client_quit = true;
					quit_listener = true;
					break;
				}
			}
		}
	}

	int Server::SendPosition(int client_num) {
		unsigned int len = sizeof(UDP_clients[client_num]);
		msg.Serialize();
		if (sendto(sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_CONFIRM, (const struct sockaddr*) &UDP_clients[client_num], len) < 0) {
			return 1;
		}
		msg.Deserialize();

		return 0;
	}

	bool Server::GetKey(int key_num) {
		return keys[key_num];
	}

	int Server::AnnounceEnd( int client_num ) {

		// VERIFY whether there are clients already or not (quitting before clients established connection )

		char client_msg[MAXLINE];
		strcpy( client_msg, USER_DESTROY );

		if ( send( TCP_clients[client_num], (const char*) client_msg, strlen(client_msg), 0 ) < 0 ) {
			perror("[Error] Failed to send quit message to a client!\n");
			return 1;
		}

		return 0;
	}

	void Server::RuntimeMessage( std::string msg ){
		std::cout << msg << std::endl;
	}

	bool Server::GetQuit() { return quit; }
	bool Server::GetClientQuit() { return client_quit; }
	void Server::QuitListener() { quit_listener = true; }
	bool Server::IsClientConnected( int client ) { return (TCP_clients[client] < 0) ? false : true; }
}
#endif