#pragma once
#ifndef PONG_H
#define PONG_H

#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Ball.h"
#include <array>

namespace Pong {
	constexpr float PADDING = 10.0f;
	constexpr int32_t DIV_SIZE = 6;
	constexpr int32_t BORDER = 1;

	enum PlayerNum { PlayerOne, PlayerTwo };

	class Pong : public olc::PixelGameEngine {
	private:
		std::array<std::unique_ptr<Player::Player>, 2> players;
		std::unique_ptr<Ball::Ball> ball;
		std::array<int, 2> score;

	private:
		void DrawDivision();

	public:
		Pong();

		bool OnUserCreate() override;
		bool OnUserUpdate(float) override;
	};
}

#endif // !PONG_H
