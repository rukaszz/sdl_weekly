#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <SDL2/SDL.h>

enum class GameScene : size_t {
    Title, 
    Playing, 
    GameOver, 
    Clear, 
    Result, 
    Count, 
};

#endif  // GAMESCENE_H
