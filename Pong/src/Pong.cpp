#include "../header/Pong.h"

namespace Pong {

	Pong::Pong() {
		sAppName = "Pong";
	}

	bool Pong::OnUserCreate() {

		players[PlayerOne] = std::make_unique<Player::Player>(Player::PlayerOne, *this );
		players[PlayerTwo] = std::make_unique<Player::Player>(Player::PlayerTwo, *this );

		ball = std::make_unique<Ball::Ball>(*this);

		score = { 0, 0 };

		return true;
	}

	bool Pong::OnUserUpdate(float fElapsedTime) {
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
		ball->CheckCollision(players[PlayerOne]->Position(), players[PlayerTwo]->Position(), score);

		// Clears the screen
		Clear(olc::BLACK);

		// Display the scenario
		DrawRect(0, 0, ScreenWidth() - BORDER, ScreenHeight() - BORDER, olc::GREEN);
		DrawDivision();

		// Display the score
		std::string scoreLine[2] = { std::string("Player 1: ") + std::to_string(score[PlayerOne]), std::string("Player 2: ") + std::to_string(score[PlayerTwo]) };
		DrawString(ScreenWidth()/4 - GetTextSize(scoreLine[PlayerOne]).x, int32_t(PADDING), scoreLine[PlayerOne], olc::GREEN, 2);
		DrawString(3*ScreenWidth()/4 - GetTextSize(scoreLine[PlayerTwo]).x, int32_t(PADDING), scoreLine[PlayerTwo], olc::GREEN, 2);

		// Displays entities
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