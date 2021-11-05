#if defined CLIENT || defined SERVER
#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

namespace Pong::Network {
	constexpr int FAIL = 1;
	int CheckPort( const std::string& );
	in_addr_t CheckIp( const std::string& );
}
namespace Pong::Network::GameInfo {
	struct GameInfo {
		int xPlayer1;
		int yPlayer1;
		int xPlayer2;
		int yPlayer2;
		int xBall;
		int yBall;
		int scorePlayer1;
		int scorePlayer2;

		void Serialize();
		void Deserialize();
	};

	
	
}

#endif  // !NETWORK_H
#endif  // CLIENT || SERVER