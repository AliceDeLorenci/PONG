#include "../header/Player.h"
#include "../header/Pong.h"

namespace Pong::Player{

	const olc::vi2d Player::size = olc::vi2d( PLAYER_WIDTH, PLAYER_HEIGHT );	// bar size
	const float Player::speed = PLAYER_SPEED;	// speed magnitude

	Player::Player( olc::vf2d initialPos, olc::PixelGameEngine& game ) : position(initialPos), pge(game) {}

	void Player::Move( olc::vf2d displacement ) {

		position += displacement;

		//Keeps the player within the arena
		if ( position.y < float(BORDER) )
			position.y = float(BORDER);
		if ( position.y > pge.ScreenHeight() - float(BORDER) - size.y )
			position.y = float( pge.ScreenHeight() - float(BORDER) - size.y );

	}

	void Player::Draw() {
		//Draw player on screen
		pge.FillRect( int(position.x), int(position.y), size.x, size.y, olc::WHITE );
	}
}
