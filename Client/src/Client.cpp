#include "../header/Client.h"

#include <errno.h>

namespace Client {

	Client::Client() {
            // Allocate a UDP socket for the client
            if ( ( my_socket = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ) {
                perror("Socket creation failure.");
                exit(EXIT_FAILURE);
            }
        
            // Server information
            memset( &server_addr, 0, sizeof(server_addr) );
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(SERVER_PORT);
            server_addr.sin_addr.s_addr = INADDR_ANY;

            memset( &msg, 0, sizeof(msg) );
    }

    int Client::Connect() {
            char buffer[MAXLINE];
            
            // Joins game
            std::cout << server_addr.sin_family << std::endl;
            std::cout << server_addr.sin_port << std::endl;
            std::cout << server_addr.sin_addr.s_addr << std::endl;

            unsigned int len = sizeof(server_addr);
            sendto( my_socket, (const char *) CONNECT_HANDSHAKE, strlen(CONNECT_HANDSHAKE), MSG_CONFIRM, 
                    (const struct sockaddr *) &server_addr, len );
            
            // Receives player number
            len = sizeof(server_addr);
            int n = recvfrom(my_socket, (char *) buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
            
            buffer[n] = '\0';
            player_num = atoi( buffer );
            std::cout << "I am: " << player_num << std::endl;
        
            return 0;
    }

   void Client::StartListening(){
        
        thread_listen = std::thread ( &Client::Listen, this );
    }

    void Client::Listen( ){
        
        while(true) {
            unsigned int len = sizeof(server_addr);

            //std::cout << "Waiting for message..." << std::endl;

            recvfrom( my_socket, &msg, sizeof(struct game_info), MSG_WAITALL, ( struct sockaddr *) &server_addr, &len );
           
            //std::cout << msg.xPlayer1 << std::endl;
        }
        
    }

    int Client::SendKeys(){
    
        errno = 0;

        // montar string
        char client_msg[MAXLINE];

        //std::cout << "UP: " << keys[UP] << " - DOWN: " << keys[DOWN] << std::endl;

        sprintf( client_msg, "KEYS%d%d%d", player_num, keys[UP], keys[DOWN] );

        //std::cout << "Try to say: " << client_msg << std::endl;

        //std::cout << "Server port: " << server_addr.sin_port << std::endl;

        // Sends game configuration

        std::cout << server_addr.sin_family << std::endl;
        std::cout << server_addr.sin_port << std::endl;
        std::cout << server_addr.sin_addr.s_addr << std::endl;

        unsigned int len = sizeof( server_addr );
        if( sendto( my_socket, (const char *) client_msg, strlen(client_msg), MSG_CONFIRM, 
                    (const struct sockaddr *) &server_addr, len ) < 0 ){
            
            perror( "Error sending msg\n" );
            return 1;
        }
        
        std::cout << "SUCESSO" << std::endl;
        return 0;
    }
    
    void Client::SetKey( int key, bool held ){
        keys[ key ] = held;
    }
}