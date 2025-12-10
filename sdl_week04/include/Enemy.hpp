#ifndef ENEMY_H
#define ENEMY_H

// 定数
#include "EnemyConfig.hpp"

#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>

class Texture;
class Renderer;

class Enemy : public Character{
private:

public:
    // 定数
    static inline constexpr int NUM_FRAMES = EnemyConfig::NUM_FRAMES;

    Enemy(Texture& tex);

    void update(double delta, DrawBounds bounds) override;
    SDL_Rect getCollisionRect() const override;

    void setSpeed(double v){
        speed = v;
    }
};

#endif  // ENEMY_H
