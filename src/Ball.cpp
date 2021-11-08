#include "Ball.h"

#include "Player.h"
#include "Pong.h"

/**
 * Methods concerning the ball.
 */
namespace Pong::Ball {
    std::random_device rd;
    std::mt19937 mt(rd());

    const olc::vi2d Ball::size = olc::vi2d(BALL_SIZE, BALL_SIZE);

    /**
     * Construct a new Ball::Ball. Calculates the initial position.
     */
    Ball::Ball(olc::PixelGameEngine& game) : pge(game) {
        // set up all random number generators
        distY = std::uniform_real_distribution<float>(0.0f, float(pge.ScreenHeight() - size.y));
        distAngle = std::uniform_real_distribution<float>(-PI / 3.0f, PI / 3.0f);
        distDirection = std::bernoulli_distribution(0.5);

        // starting position and speed
        Reset();
    }
    Ball::~Ball() {}

    /**
     * Reset ball position and speed.
     */
    void Ball::Reset() {
        // middle of the screen with height ranging from 1/4 to 3/4 of the ScreenHeight()
        position = olc::vf2d((pge.ScreenWidth() - size.x) / 2.0f, pge.ScreenHeight() / 4.0f + distY(mt) / 2.0f);

        // random direction in the range of (-60; 60) to (120, 240) degree
        float fAngle = distAngle(mt) + float(distDirection(mt)) * PI;
        direction = olc::vf2d(cos(fAngle), sin(fAngle));

        // initial speed (the speed increases throughout the game)
        curSpeed = INITIAL_SPEED;
    }

    /**
     * Moves the ball according to the elapsed time.
     */
    void Ball::Move(float fElapsedTime) {
        position += direction * curSpeed * fElapsedTime;
    }

    /**
     * Checks whether the ball collided with a player or with the screen delimiters or whether it has 
     * exited the screen.
     */
    void Ball::CheckCollision(const olc::vf2d& player1Pos, const olc::vf2d& player2Pos, std::array<int, 2>& score) {
        using namespace std::chrono;
        static auto lastCollisionTime = high_resolution_clock::now();   
        auto curTime = high_resolution_clock::now();

        // determines whether the ball collided with the players
        auto testResolveCollision = [&](const olc::vf2d& player) {
            if (position.x < player.x + Player::Player::size.x &&
                position.x + size.x > player.x &&
                position.y < player.y + Player::Player::size.y &&
                position.y + size.y > player.y) {
                // Limits the number of player collision per seconds the ball can have. Prevents increasing the speed too fast.
                // To travel a quarter of the screen, it takes ~ ScreenWidth()/(MAX_SPEED*4) seconds, so we'll be using that as a hard limit.
                if (duration_cast<milliseconds>(curTime - lastCollisionTime).count() < static_cast<int64_t>((1000.0f * pge.ScreenWidth()) / (4.0f * MAX_SPEED)))
                    return;
                lastCollisionTime = high_resolution_clock::now();

                // the ball is deflected by a random angle
                float fAngle = distAngle(mt) + float(distDirection(mt)) * PI;
                direction = olc::vf2d(cos(fAngle), sin(fAngle));
                direction.x = (position.x > pge.ScreenWidth() / 2.0f) ? -std::abs(direction.x) : std::abs(direction.x);

                IncreaseSpeed();
            }
        };

        // collision with the players
        testResolveCollision(player1Pos);
        testResolveCollision(player2Pos);

        // collision with top and bottom borders
        if (position.y <= float(BORDER)) {
            position.y = float(BORDER);
            direction.y = std::abs(direction.y);
        } else if (position.y >= float(pge.ScreenHeight() - BORDER - size.y)) {
            position.y = float(pge.ScreenHeight() - float(BORDER) - size.y);
            direction.y = -std::abs(direction.y);
        }

        // ball went out of the screen (compute new score)
        if (position.x <= 0.0f) {
            score[PlayerTwo]++;
            Reset();
        } else if (position.x >= pge.ScreenWidth() - size.x) {
            score[PlayerOne]++;
            Reset();
        }
    }

    /**
     * The ball speed increases as it collides with the players, until it reaches a maximum value.
     */
    void Ball::IncreaseSpeed() {
        curSpeed *= 1.1f;
        curSpeed = (curSpeed <= MAX_SPEED) ? curSpeed : MAX_SPEED;
    }

    /**
     * Draws the ball on the screen.
     */
    void Ball::Draw() {
        pge.FillRect(int32_t(position.x), int32_t(position.y), size.x, size.y, olc::WHITE);
    }

    /**
     * Getters.
     */
    const olc::vf2d& Ball::Position() { return position; };
    const olc::vf2d& Ball::Direction() { return direction; };
    const float& Ball::CurSpeed() { return curSpeed; };

    /**
     * Sets the ball position.
     */
    void Ball::SetPosition(int x, int y) {
        position.x = x;
        position.y = y;
    }
}  // namespace Pong::Ball