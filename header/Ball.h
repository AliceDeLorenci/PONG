#pragma once
#ifndef BALL_H
#define BALL_H

#include <random>

#include "olcPixelGameEngine.h"

namespace Pong::Ball {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float INITIAL_SPEED = 250.0f;
    constexpr float MAX_SPEED = 750.0f;
    constexpr int32_t BALL_SIZE = 8;

    class Ball {
     private:
        olc::PixelGameEngine& pge;                          // Game engine

        // Random number generators
        std::uniform_real_distribution<float> distY;        // (0 ~ ScreenHeight())
        std::uniform_real_distribution<float> distAngle;    // (pi/4 ~ 3pi/4)
        std::bernoulli_distribution distDirection;          // (0, 1) * PI

        olc::vf2d position;                                 // Relative to anchor on top left corner
        olc::vf2d direction;                                // Movement direction (normalized)
        float curSpeed;                                     // Speed magnitude

     public:
        static const olc::vi2d size;      // Size in pixels

     private:
        void Reset();                     // Repositions the ball

     public:
        Ball(olc::PixelGameEngine&);
        virtual ~Ball();

        void Move(float);
        void CheckCollision(const olc::vf2d&, const olc::vf2d&, std::array<int, 2>&);
        void IncreaseSpeed();
        void Draw();

        // Getters
        const olc::vf2d& Position();
        const olc::vf2d& Direction();
        const float& CurSpeed();

        // Setters
        void SetPosition(int, int);
    };
}  // namespace Pong::Ball

#endif  // !BALL_H
