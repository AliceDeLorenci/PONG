#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "olcPixelGameEngine.h"

namespace Pong::Player {

	constexpr float PLAYER_SPEED = 200.0f;
	constexpr int PLAYER_WIDTH = 8;
	constexpr int PLAYER_HEIGHT = 35;

	class Player {
	private:
		olc::PixelGameEngine& pge;
	public:
		olc::vf2d position; 			// Anchor on top left corner
		static const olc::vi2d size;	// Player size in pixels
		static const float speed;		// Player speed magnitude

		Player(olc::vf2d initialPos, olc::PixelGameEngine& game);

		void Move(olc::vf2d displacement);	

		void Draw();
	};
}

#endif // !PLAYER_H