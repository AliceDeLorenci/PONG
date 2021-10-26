#include "../header/Server.h"
//#include <boost/asio/impl/src.hpp>

namespace Pong::Server {

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

    
        if( bind( my_socket, (struct sockaddr*)&addr, sizeof(addr) ) < 0 ){
            perror("Bind failed.");
            exit(EXIT_FAILURE);
        }   

        // Socket on passive open mode
        listen( my_socket, 1 );   

        // debug
        std::cout << "wainting client" << endl;
	}
}