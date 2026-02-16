#ifndef ENEMY_H
#define ENEMY_H

// 設定系
#include "GameConfig.hpp"
#include "EnemyConfig.hpp"
#include "EnemySensor.hpp"
// 設計系
#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"
// SDL
#include <SDL2/SDL.h>
// std
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
    bool onGround = false;  // 敵が床上にいるか
public:
    // 定数
    static inline constexpr int NUM_FRAMES = EnemyConfig::NUM_FRAMES;
    // コンストラクタ
    Enemy(Texture& tex);
    ~Enemy() = default;
    // InputStateは使用しないので無名
    void update(double delta, const InputState&, DrawBounds bounds, const std::vector<Block>& blocks) override;
    // 当たり判定を返す関数
    SDL_Rect getCollisionRect() const override;
    // 描画
    void draw(Renderer& renderer, Camera& camera) override;
    // 乱数を用いてx, y, speedを設定する関数
    void reset(
        std::mt19937& rd,
        std::uniform_real_distribution<double>& dx,
        std::uniform_real_distribution<double>& dy,
        std::uniform_real_distribution<double>& ds
    );
    // EnemyStateをDyingにする関数
    void startDying();
    // スポーン時にx, y, speedをセットする
    void applyEnemyParamForSpawn(double coorX, double coorY, double spd);
    
    // getter/setter
    void setSpeed(double v){
        speed = v;
    }
    bool isOnGround() const noexcept{
        return onGround;
    }
    EnemyState getState() const{ return state; }
    bool isAlive() const{ return state == EnemyState::Alive; };
    bool isDying() const{ return state == EnemyState::Dying; };
    bool isDead()  const{ return state == EnemyState::Dead; };
public:
    // update()内の処理を分割するための関数群
    // 敵の行動判断用
    virtual void think(double delta, const EnemySensor& es){}
    // 物理処理用
    virtual void stepPhysics(double delta, DrawBounds bounds, const std::vector<Block>& blocks);
    // アニメーション更新用
    virtual void updateAnimation(double delta);
};

#endif  // ENEMY_H
