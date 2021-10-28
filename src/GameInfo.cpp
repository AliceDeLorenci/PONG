#if defined CLIENT || defined SERVER
#include "GameInfo.h"
#include <netinet/in.h>
#include <arpa/inet.h>

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