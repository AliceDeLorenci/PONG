#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "olcPixelGameEngine.h"

namespace Pong::Player {
    constexpr float PLAYER_SPEED = 300.0f;
    constexpr int32_t PLAYER_WIDTH = 8;
    constexpr int32_t PLAYER_HEIGHT = 35;

    enum PlayerNum { PlayerOne,
                     PlayerTwo };

    class Player {
     private:
        olc::PixelGameEngine& pge;          // Game engine
        olc::vf2d position;                 // Relative to anchor on top left corner
        PlayerNum number;                   // Player number

     public:
        static const olc::vi2d size;        // Player size in pixels
        static const float speed;           // Player speed magnitude

        Player(const PlayerNum playerNumber, olc::PixelGameEngine& game);
        virtual ~Player();

        void Move(const olc::vf2d&);
        void Draw();

        // Getters
        const olc::vf2d& Position();

        // Setters
        void SetPosition(int, int);
    };
}  // namespace Pong::Player

#endif  // !PLAYER_H