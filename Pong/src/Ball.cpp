#include "../header/Ball.h"
#include "../header/Player.h"
#include "../header/Pong.h"

namespace Pong::Ball {
	std::random_device rd;
	std::mt19937 mt(rd());

	const olc::vi2d Ball::size = olc::vi2d(BALL_SIZE, BALL_SIZE);

	Ball::Ball(olc::PixelGameEngine& game) : pge(game) {
		// Set up all RNG related variables
		distY = std::uniform_real_distribution<float>(0.0f, float(pge.ScreenHeight() - size.y));
		distAngle = std::uniform_real_distribution<float>(-PI / 3.0f, PI / 3.0f);
		distDirection = std::bernoulli_distribution(0.5);

		// Starting position and speed
		Reset();
	}

	// Reset ball position and speed
	void Ball::Reset() {
		// Middle of the screen with height ranging from 1/4 to 3/4 of the ScreenHeight()
		position = olc::vf2d((pge.ScreenWidth() - size.x) / 2.0f, pge.ScreenHeight() / 4.0f + distY(mt) / 2.0f);

		// Random direction in the range of (-60; 60) to (120, 240) degree
		float fAngle = distAngle(mt) + float(distDirection(mt)) * PI;
		direction = olc::vf2d(cos(fAngle), sin(fAngle));

		curSpeed = INITIAL_SPEED;
	}

	void Ball::Move(float fElapsedTime) {
		position += direction * curSpeed * fElapsedTime;
	}

	void Ball::CheckCollision(const olc::vf2d& player1Pos, const olc::vf2d& player2Pos, std::array<int, 2>& score) {
		auto testResolveCollision = [&](const olc::vf2d& player) {
			if (position.x				< player.x + Player::Player::size.x &&
				position.x + size.x		> player.x &&
				position.y				< player.y + Player::Player::size.y &&
				position.y + size.y		> player.y) {
				// angulo aleatorio:
				float fAngle = distAngle(mt) + float(distDirection(mt)) * PI;
				direction = olc::vf2d(cos(fAngle), sin(fAngle));
				direction.x = (position.x > pge.ScreenWidth() / 2.0f) ? -std::abs(direction.x) : std::abs(direction.x);

				IncreaseSpeed();
			}
		};

		// Collision with the Player
		testResolveCollision(player1Pos);
		testResolveCollision(player2Pos);

		// Collision with top and bottom borders
		if (position.y <= float(BORDER)) {
			position.y = float(BORDER);
			direction.y = std::abs(direction.y);
		}
		else if (position.y >= float(pge.ScreenHeight() - BORDER - size.y)) {
			position.y = float(pge.ScreenHeight() - float(BORDER) - size.y);
			direction.y = -std::abs(direction.y);
		}

		// Collision with left and right borders (Score)
		if (position.x <= 0.0f) {
			score[PlayerTwo]++;
			Reset();
		}
		else if (position.x >= pge.ScreenWidth() - size.x) {
			score[PlayerOne]++;
			Reset();
		}
	}

	void Ball::IncreaseSpeed() {
		curSpeed *= 1.1f;
		curSpeed = (curSpeed <= MAX_SPEED) ? curSpeed : MAX_SPEED;
	}

	void Ball::Draw() {
		pge.FillRect(int32_t(position.x), int32_t(position.y), size.x, size.y, olc::WHITE);
	}

	const olc::vf2d& Ball::Position() { return position; };
	const olc::vf2d& Ball::Direction() { return direction; };
	const float& Ball::CurSpeed() { return curSpeed; };
}