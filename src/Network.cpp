#if defined CLIENT || defined SERVER
#include "Network.h"


namespace Pong::Network{
	int CheckPort( const std::string& port ){
		int ret_port;
		 
		if(!port.empty() && port.find_first_not_of("0123456789") == std::string::npos) { // if port is a number
			ret_port = std::stoi(port);
			if(ret_port <= 0 || ret_port > 65535) {
				std::cout << "[Error]: Port must be in range (0, 65535]!" << std::endl;
				exit(FAIL);
			}
		}
		else {
			std::cout << "[Error]: Port must be a number!" << std::endl;
			exit(FAIL);
		}
		return ret_port;
	}

	in_addr_t CheckIp( const std::string& ip) {
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
		return convertedIp;
	}
}

namespace Pong::Network::GameInfo {
	void GameInfo::Serialize() {
		xPlayer1 = htonl(xPlayer1);
		yPlayer1 = htonl(yPlayer1);
		xPlayer2 = htonl(xPlayer2);
		yPlayer2 = htonl(yPlayer2);
		xBall = htonl(xBall);
		yBall = htonl(yBall);
		scorePlayer1 = htonl(scorePlayer1);
		scorePlayer2 = htonl(scorePlayer2);
	}
	void GameInfo::Deserialize() {
		xPlayer1 = ntohl(xPlayer1);
		yPlayer1 = ntohl(yPlayer1);
		xPlayer2 = ntohl(xPlayer2);
		yPlayer2 = ntohl(yPlayer2);
		xBall = ntohl(xBall);
		yBall = ntohl(yBall);
		scorePlayer1 = ntohl(scorePlayer1);
		scorePlayer2 = ntohl(scorePlayer2);
	}
}

#endif