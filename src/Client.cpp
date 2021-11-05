#if CLIENT
#include "Client.h"

#include <errno.h>

namespace Pong::Network::Client {
	Client::Client(const std::string& ServerIp, const std::string& UDPServerPort, const std::string& TCPServerPort) {
		
		ports[UDP] = CheckPort( UDPServerPort );
		ports[TCP] = CheckPort( TCPServerPort );
		auto convertedIp = CheckIp(ServerIp);
		
		// Server information
		for( int TYPE = UDP; TYPE <= TCP; TYPE++ ){
			memset( &server_addr[TYPE], 0, sizeof(server_addr[TYPE]) );
			server_addr[TYPE].sin_family = AF_INET;
			server_addr[TYPE].sin_port = htons( ports[TYPE] );
			server_addr[TYPE].sin_addr.s_addr = convertedIp;
		}

		memset(&msg, 0, sizeof(msg));

		server_quit = false;
		quit_listener = false;
		quit = false;
	}
	Client::~Client() {}

	int Client::Connect() {

		// TCP
		// Server: "<player number>"
		sockets[TCP] = socket( AF_INET, SOCK_STREAM, 0 );

		if( connect( sockets[TCP], (struct sockaddr*) &server_addr[TCP], sizeof(server_addr[TCP]) ) == -1 ){
			perror("[Error] Failed to stablish a TCP connection. The server may be disconnected.\n");
			return 1;
		}

		// Receive player number
		char incoming_msg[MAXLINE];
		int n = recv( sockets[TCP], incoming_msg, MAXLINE, 0 );
    	incoming_msg[n] = '\0';
		player_num = atoi( incoming_msg );

		// UDP
		// Client: "I am <player number>"
		sockets[UDP] = socket( AF_INET, SOCK_DGRAM, 0 );
		char buffer[MAXLINE];
		sprintf( buffer, "I AM %d", player_num );

		unsigned int len = sizeof( server_addr[UDP] );
		sendto( sockets[UDP], buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr*) &server_addr[UDP], len );

		return 0;
	}

	void Client::StartListening() {
		thread_listen[UDP] = std::thread(&Client::ListenUDP, this);
		thread_listen[UDP].detach();

		thread_listen[TCP] = std::thread(&Client::ListenTCP, this);
		thread_listen[TCP].detach();
	}

	void Client::ListenUDP() {
		while ( !quit_listener ) {
			unsigned int len = sizeof(server_addr[UDP]);
			recvfrom( sockets[UDP], &msg, sizeof(Pong::Network::GameInfo::GameInfo), MSG_WAITALL, (struct sockaddr*) &server_addr[UDP], &len);
			msg.Deserialize();
		}
	}

	void Client::ListenTCP() {
		
		fcntl(sockets[TCP], F_SETFL, fcntl(sockets[TCP], F_GETFL) | O_NONBLOCK);
		while ( !quit_listener ) {
			char buffer[MAXLINE];
			int n = recv( sockets[TCP], buffer, MAXLINE, 0 );
			if(n == -1) continue;
			buffer[n] = '\0';
			
			if ( strncmp( USER_DESTROY, buffer, strlen(USER_DESTROY) ) == 0 ) {

				RuntimeMessage( "[STATUS] Server disconnected" );

				quit = true;
				server_quit = true;
				quit_listener = true;
			}
			
		}
	}

	int Client::SendKeys() {

		// build string
		char client_msg[MAXLINE];
		sprintf(client_msg, "KEYS %d %d %d", player_num, keys[UP], keys[DOWN]);

		// Sends game configuration
		unsigned int len = sizeof( server_addr[UDP] );
		if (sendto( sockets[UDP], (const char*) client_msg, strlen(client_msg), MSG_CONFIRM, (const struct sockaddr*) &server_addr[UDP], len) < 0) {
			perror("[Error] Failed to send key presses!\n");
			return 1;
		}
		return 0;
	}

	void Client::SetKey(int key, bool held) {
		keys[key] = held;
	}

	int Client::AnnounceEnd( ){
	
		// build string
		char client_msg[MAXLINE];
		strcpy( client_msg, USER_DESTROY );

		if ( send(sockets[TCP], (const char*) client_msg, strlen(client_msg), 0 ) < 0 ) {
			perror("[Error] Failed to send quit msg to the server\n");
			return 1;
		}

		return 0;
	}

	void Client::RuntimeMessage( std::string msg ){
		std::cout << msg << std::endl;
	}

	bool Client::GetQuit(){ return quit; }
	bool Client::GetServerQuit(){ return server_quit; }
	void Client::QuitListener(){ quit_listener = true; }
}
#endif // CLIENT