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

	// Reset ball position
	void Ball::Reset() {

		// Middle of the screen with height ranging from 1/4 to 3/4 of the ScreenHeight()
		position = olc::vf2d((pge.ScreenWidth() - size.x) / 2.0f, pge.ScreenHeight() / 4.0f + distY(mt) / 2.0f);

		// Random direction in the range of (-60; 60) to (120, 240) degree
		float fAngle = distAngle(mt) + distDirection(mt) * PI;
		direction = olc::vf2d(cos(fAngle), sin(fAngle));

		speed = INITIAL_SPEED;
	}

	void Ball::Move(float fElapsedTime) {
		// Calculate new position
		position += direction * speed * fElapsedTime;
	}

	void Ball::Draw() {
		pge.FillRect(int(position.x), int(position.y), size.x, size.y, olc::WHITE);
	}

	void Ball::CheckCollision(olc::vf2d player1_position, olc::vf2d player2_position, std::array<int, 2>& score) {

		auto testResolveCollision = [&](const olc::vf2d& player) {
			if (position.x				< player.x + Player::Player::size.x &&
				position.x + size.x		> player.x &&
				position.y				< player.y + Player::Player::size.y &&
				position.y + size.y		> player.y) {

				direction.x = (position.x > pge.ScreenWidth() / 2) ? -abs(direction.x) : abs(direction.x);
				direction.y = (position.y + size.y / 2 > player.y + Player::Player::size.y / 2) ? -abs(direction.y) : abs(direction.y);
			}
		}; 
		
		// Collision with the Player
		testResolveCollision(player1_position);
		testResolveCollision(player2_position);

		// Collision with top and bottom borders
		if (position.y <= float(BORDER)) {
			position.y = float(BORDER);
			direction.y = abs(direction.y);
		}
		else if (position.y >= pge.ScreenHeight() - float(BORDER) - size.y) {
			position.y = float(pge.ScreenHeight() - float(BORDER) - size.y);
			direction.y = -abs(direction.y);
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


	void Ball::AdjustSpeed() {
		speed *= 1.1f;
		if(speed >= MAX_SPEED) speed = MAX_SPEED;
	}

}