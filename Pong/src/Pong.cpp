#include "../header/Pong.h"

namespace Pong {

	Pong::Pong() {
		sAppName = "Pong";
	}

	bool Pong::OnUserCreate() {
		
		float ycoord = float(ScreenHeight())/ 2.0f - float(Player::Player::size.y) / 2.0f;

		players[PlayerOne] = std::make_unique<Player::Player>( olc::vf2d( PADDING, ycoord ), *this );
		players[PlayerTwo] = std::make_unique<Player::Player>( olc::vf2d( ScreenWidth() - PADDING - Player::Player::size.x, ycoord ), *this );

		ball = std::make_unique<Ball::Ball>(*this);

		score = { 0, 0 };

		return true;
	}

	bool Pong::OnUserUpdate(float fElapsedTime) {
		// Clears the screen
		Clear(olc::BLACK);

		// Border
		DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);

		// User input
		if (GetKey(olc::Key::W).bHeld)
			players[PlayerOne]->Move( olc::vf2d(0, -Player::Player::speed * fElapsedTime) );

		if (GetKey(olc::Key::S).bHeld)
			players[PlayerOne]->Move( olc::vf2d(0, +Player::Player::speed * fElapsedTime) );

		if (GetKey(olc::Key::UP).bHeld)
			players[PlayerTwo]->Move( olc::vf2d(0, -Player::Player::speed * fElapsedTime) );

		if (GetKey(olc::Key::DOWN).bHeld)
			players[PlayerTwo]->Move( olc::vf2d(0, +Player::Player::speed * fElapsedTime) );

		// Moves the ball
		ball->Move(fElapsedTime);

		// Checks whether the ball colided with the players
		ball->CheckCollision(players[PlayerOne]->position, players[PlayerTwo]->position, score);

		// Prints the score
		std::string scoreLine[2] = { std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo]) };
		DrawString(ScreenWidth()/4 - GetTextSize(scoreLine[PlayerOne]).x, PADDING, scoreLine[PlayerOne], olc::GREEN, 2);
		DrawString(3*ScreenWidth()/4 - GetTextSize(scoreLine[PlayerTwo]).x, PADDING, scoreLine[PlayerTwo], olc::GREEN, 2);

		// Displays entities
		DrawDivision();
		players[PlayerOne]->Draw();
		players[PlayerTwo]->Draw();
		ball->Draw();

		

		return true;
	}

	void Pong::DrawDivision(){
		int n = int( float( ScreenHeight() )/float( DIV_SIZE ) );
		int x = ScreenWidth()/2 - DIV_SIZE/4;
		int y = 0;
		for( int i=0; i<n; i+=2 ){
			FillRect( x, y, DIV_SIZE/2, DIV_SIZE, olc::WHITE);
			y += 2*DIV_SIZE;
		}
	}
}