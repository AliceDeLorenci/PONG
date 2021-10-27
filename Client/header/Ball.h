#pragma once
#ifndef BALL_H
#define BALL_H

#include "olcPixelGameEngine.h"
#include <random>

namespace Pong::Ball {
	constexpr float PI = 3.14159265358979323846f;
	constexpr float INITIAL_SPEED = 250.0f;
	constexpr float MAX_SPEED = 750.0f;
	constexpr int32_t BALL_SIZE = 8;

	class Ball {
	private:
		// The gamespace
		olc::PixelGameEngine& pge;

		// Random Number Generator
		std::uniform_real_distribution<float> distY;		// (0 ~ ScreenHeight())
		std::uniform_real_distribution<float> distAngle;	// (pi/4 ~ 3pi/4)
		std::bernoulli_distribution distDirection;			// (0, 1) * PI

		olc::vf2d position;		// Anchor on top left corner
		olc::vf2d direction;	// Movement direction (normalized)
		float curSpeed;			// Speed magnitude

	public:
		/// Size in pixels
		static const olc::vi2d size;

	private:
		// Repositions the ball
		void Reset();

	public:
		Ball(olc::PixelGameEngine&);

		void Move(float);
		void CheckCollision(const olc::vf2d&, const olc::vf2d&, std::array<int, 2>&);
		void IncreaseSpeed();
		void Draw();

		// Getters
		const olc::vf2d& Position();
		const olc::vf2d& Direction();
		const float& CurSpeed();

		// Setters
		void SetPosition( int, int );
	};
}

#endif // !BALL_H
