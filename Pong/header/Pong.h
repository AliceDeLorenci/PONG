#pragma once
#ifndef PONG_H
#define PONG_H

#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Ball.h"
#include <array>

namespace Pong {

	constexpr float PADDING = 10.0f;
	constexpr int	DIV_SIZE = 6;
	constexpr int BORDER = 1;

	enum PlayerNum { PlayerOne, PlayerTwo };

	class Pong : public olc::PixelGameEngine {
	private:
		std::array<std::unique_ptr<Player::Player>, 2> players;
		std::unique_ptr<Ball::Ball> ball;

		std::array<int, 2> score;

	public:
		Pong();

		bool OnUserCreate() override;

		bool OnUserUpdate(float fElapsedTime) override;

		void DrawDivision();

	};

}

#endif // !PONG_H

