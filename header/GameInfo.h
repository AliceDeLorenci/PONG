#if defined CLIENT || defined SERVER
#pragma once
#ifndef GAMEINFO_H
#define GAMEINFO_H

namespace Pong::Network::GameInfo {
	struct GameInfo {
		char type[5];
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

#endif  // !GAMEINFO_H
#endif  // CLIENT || SERVER