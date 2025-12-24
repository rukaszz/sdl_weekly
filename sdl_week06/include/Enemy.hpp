#ifndef ENEMY_H
#define ENEMY_H

// 定数
#include "GameConfig.hpp"
#include "EnemyConfig.hpp"

#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>

#include <random>
#include <vector>

class Texture;
class Renderer;
struct Block;

class Enemy : public Character{
private:

public:
    // 定数
    static inline constexpr int NUM_FRAMES = EnemyConfig::NUM_FRAMES;

    Enemy(Texture& tex);
    // InputStateは使用しないので無名
    void update(double delta, const InputState&, DrawBounds bounds, const std::vector<Block>&) override;
    SDL_Rect getCollisionRect() const override;
    void reset(
        std::mt19937& rd,
        std::uniform_real_distribution<double>& dx,
        std::uniform_real_distribution<double>& dy,
        std::uniform_real_distribution<double>& ds
    );

    void setSpeed(double v){
        speed = v;
    }
};

#endif  // ENEMY_H
