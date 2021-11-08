#include "Player.h"

#include "Pong.h"

/**
 * Methods concerning the player.
 */
namespace Pong::Player {
    const olc::vi2d Player::size = olc::vi2d(PLAYER_WIDTH, PLAYER_HEIGHT);  // bar size
    const float Player::speed = PLAYER_SPEED;                               // speed magnitude

    /**
     * Constructs a new Player::Player. Calculates the initial position.
     */
    Player::Player(const PlayerNum playerNumber, olc::PixelGameEngine& game) : pge(game), number(playerNumber) {
        if (number == PlayerOne) {
            position = olc::vf2d(PADDING, pge.ScreenHeight() / 2.0f - size.y / 2.0f);
        } else {
            position = olc::vf2d(pge.ScreenWidth() - PADDING - size.x, pge.ScreenHeight() / 2.0f - size.y / 2.0f);
        }
    }
    Player::~Player() {}

    /**
     * Moves the player according to the displacement calculated by the engine.
     */
    void Player::Move(const olc::vf2d& displacement) {
        position += displacement;

        // keeps the player within the arena
        if (position.y < float(BORDER))
            position.y = float(BORDER);
        if (position.y > float(pge.ScreenHeight() - BORDER - size.y))
            position.y = float(pge.ScreenHeight() - BORDER - size.y);
    }

    /**
     * Draws the player on the screen.
     */
    void Player::Draw() {
        pge.FillRect(int32_t(position.x), int32_t(position.y), size.x, size.y, olc::WHITE);
    }

    /**
     * Returns the player's position.
     */
    const olc::vf2d& Player::Position() { return position; }

    /**
     * Sets the player's postion.
     */
    void Player::SetPosition(int x, int y) {
        position.x = x;
        position.y = y;
    }
}  // namespace Pong::Player