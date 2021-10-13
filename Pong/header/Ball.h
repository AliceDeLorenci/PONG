#pragma once
#ifndef BALL_H
#define BALL_H

#include "olcPixelGameEngine.h"
#include <random>

namespace Pong::Ball {

	constexpr float INITIAL_SPEED = 300.0f;
	constexpr float MAX_SPEED = 500.0f;
	constexpr float PI = 3.14159265358979323846f;
	constexpr int BALL_SIZE = 8;

	class Ball {
	private:
		// The gamespace
		olc::PixelGameEngine& pge;

		// Random Number Generator
		std::uniform_real_distribution<float> distY;		// (0 ~ ScreenHeight())
		std::uniform_real_distribution<float> distAngle;	// (pi/4 ~ 3pi/4)
		std::bernoulli_distribution distDirection;			// (0, 1) * PI

	public:
		/// Size in pixels
		static const olc::vi2d size;

		olc::vf2d position;		// Anchor on top left corner
		olc::vf2d direction;	// Movement direction (normalized)
		float speed;			// Movement magnitude

	private:
		// Repositions the ball
		void Reset();

	
	public:
		Ball(olc::PixelGameEngine&);

		void Move(float);
		void Draw();
		void CheckCollision(olc::vf2d, olc::vf2d, std::array<int, 2>&);
		void AdjustSpeed();

	};
}

#endif // !BALL_H
