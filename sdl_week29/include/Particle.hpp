#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL2/SDL.h>

/**
 * @brief パーティクル管理用構造体
 * 
 */
struct Particle{
    double x;
    double y;
    double vx;
    double vy;
    double life;
    double maxLife;
    int size;
    SDL_Color color;
};

#endif  // PARTICLE_H
