#include "../header/Player.h"
#include "../header/Pong.h"

namespace Pong::Player {
	const olc::vi2d Player::size = olc::vi2d(PLAYER_WIDTH, PLAYER_HEIGHT); // bar size
	const float Player::speed = PLAYER_SPEED; // speed magnitude

	Player::Player(const PlayerNum playerNumber, olc::PixelGameEngine& game) : pge(game), number(playerNumber) {
		if (number == PlayerOne) {
			position = olc::vf2d(PADDING, pge.ScreenHeight() / 2.0f - size.y / 2.0f);
		}
		else {
			position = olc::vf2d(pge.ScreenWidth() - PADDING - size.x, pge.ScreenHeight() / 2.0f - size.y / 2.0f);
		}
	}

	void Player::Move(const olc::vf2d& displacement) {
		position += displacement;

		// Keeps the player within the arena
		if (position.y < float(BORDER))
			position.y = float(BORDER);
		if (position.y > float(pge.ScreenHeight() - BORDER - size.y))
			position.y = float(pge.ScreenHeight() - BORDER - size.y);
	}

	void Player::Draw() {
		pge.FillRect(int32_t(position.x), int32_t(position.y), size.x, size.y, olc::WHITE);
	}

	const olc::vf2d& Player::Position() { return position; }

	void Player::SetPosition( int x, int y ){
		position.x = x;
		position.y = y;
	}
}