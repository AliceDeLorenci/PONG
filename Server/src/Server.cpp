#include "../header/Server.h"


namespace Server {

	Server::Server() {
        // Allocate a UDP socket for the server
        if( ( my_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ){
            perror("Socket creation failure.");
            exit(EXIT_FAILURE);
        }      

        // Assign a port and IP address to the server socket
        struct sockaddr_in addr;
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons( SERVER_PORT );
        addr.sin_addr.s_addr = INADDR_ANY;
        memset( &addr.sin_zero, 0, sizeof(addr.sin_zero) );

        // Bind socket to port
        if( bind( my_socket, (struct sockaddr*)&addr, sizeof(addr) ) < 0 ){
            perror("Bind failed.");
            exit(EXIT_FAILURE);
        }   
        
	}

    int Server::AcceptClient( int client_num ) {
        
        // Accepts a client and saves its address for future reference
        char buffer[MAXLINE];
        unsigned len = sizeof( &( clients[client_num] ) );
        int n = recvfrom( my_socket, (char *) buffer, MAXLINE, MSG_WAITALL, 
                            ( struct sockaddr *) &( clients[client_num] ), &len );
        buffer[n] = '\0';
        
        if( strcmp( buffer, NEW_CLIENT ) )
            return 1;

        // Says which player the client is
        const char* client_msg = std::to_string(client_num).c_str();

        sendto( my_socket, client_msg, strlen(client_msg), MSG_CONFIRM, 
                (const struct sockaddr *) &( clients[client_num] ), len );
        return 0;
    }

    void Server::StartListening(){
        
        thread_listen = std::thread ( &Server::Listen, this );
    }

    void Server::Listen( ){

        struct sockaddr_in client_addr;

        while(true) {
            char buffer[MAXLINE];
            unsigned int len = sizeof(client_addr);
            memset( &client_addr, 0, sizeof(client_addr) );
            int n = recvfrom( my_socket, (char *) buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &len );
            
            buffer[n] = '\0';
            printf( "%s\n", buffer );

            if( strncmp( INCOMING_KEYS, buffer, strlen( INCOMING_KEYS ) ) == 0 ){
                char type[5];
                int isPlayerOne;
                int up;
                int down;

                sscanf( buffer, "%s %d %d %d", type, &isPlayerOne, &up, &down );
                
                //bool isPlayerOne = atoi( &( buffer[ strlen( INCOMING_KEYS ) ] ) );
                //bool up = atoi( &( buffer[ strlen( INCOMING_KEYS )+1 ] ) );
                //bool down = atoi( &( buffer[ strlen( INCOMING_KEYS )+2 ] ) );

                if( up | down ){
                    printf( "UP: %d - DOWN: %d\n", up, down );
                }
                
                if( isPlayerOne ){
                    keys[W] = up;
                    keys[S] = down;
                }else{
                    keys[UP] = up;
                    keys[DOWN] = down;
                }
            }

            /*
            std::string buf(buffer);
            if( buf.compare( 0, INCOMING_KEYS.length(), INCOMING_KEYS ) == 0 ){

                constexpr size_t playerIndex = INCOMING_KEYS.length();
                bool isPlayerOne = !static_cast<bool>(buffer[playerIndex]);

                constexpr size_t upIndex = playerIndex + 1;
                bool up = static_cast<bool>(buffer[upIndex]);
                
                constexpr size_t downIndex = playerIndex + 2;
                bool down = static_cast<bool>(buffer[downIndex]);

                if( up | down ){
                    printf( "UP: %d - DOWN: %d\n", up, down );
                }
                
                if( isPlayerOne ){
                    keys[W] = up;
                    keys[S] = down;
                }else{
                    keys[UP] = up;
                    keys[DOWN] = down;
                }
                
            }
            */
        }
        
    }

    int Server::SendPosition( int client_num ){

        // Sends game configuration
        unsigned int len = sizeof( clients[client_num] );
        if( sendto( my_socket, &msg, sizeof(struct game_info), MSG_CONFIRM, (const struct sockaddr *) &clients[client_num], len ) < 0 ){
            return 1;
        }
        
        return 0;

        /*
        // montar string
        char client_msg[MAXLINE];

        sprintf( client_msg, "CONF %d %d %d %d %d %d %d %d", xPlayer1, yPlayer1, xPlayer2, yPlayer2, xBall, yBall, scorePlayer1, scorePlayer2 );

        // Sends game configuration
        unsigned int len = sizeof( clients[client_num] );
        if( sendto( my_socket, client_msg, strlen(client_msg), MSG_CONFIRM, (const struct sockaddr *) &clients[client_num], len ) < 0 ){
            return 1;
        }

        return 0;
        */
    }

    bool Server::GetKey( int key_num ){
        return keys[ key_num ];
    }
    
}