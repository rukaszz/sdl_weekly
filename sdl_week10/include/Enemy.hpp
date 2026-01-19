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

// Enemyの状態管理
enum class EnemyState{
    Alive, 
    Dying, 
    Dead, 
};

class Enemy : public Character{
private:
    // 死亡演出関係
    EnemyState state = EnemyState::Alive;
    double dyingTime = 0.0; // Dying状態の経過時間
public:
    // 定数
    static inline constexpr int NUM_FRAMES = EnemyConfig::NUM_FRAMES;

    Enemy(Texture& tex);
    // InputStateは使用しないので無名
    void update(double delta, const InputState&, DrawBounds bounds, const std::vector<Block>&) override;
    SDL_Rect getCollisionRect() const override;
    void draw(Renderer& renderer, Camera& camera) override;
    void reset(
        std::mt19937& rd,
        std::uniform_real_distribution<double>& dx,
        std::uniform_real_distribution<double>& dy,
        std::uniform_real_distribution<double>& ds
    );
    void startDying();
    // getter/setter
    void setSpeed(double v){
        speed = v;
    }
    EnemyState getState() const{ return state; }
    bool isAlive() const{ return state == EnemyState::Alive; };
    bool isDying() const{ return state == EnemyState::Dying; };
    bool isDead()  const{ return state == EnemyState::Dead; };
};

#endif  // ENEMY_H
