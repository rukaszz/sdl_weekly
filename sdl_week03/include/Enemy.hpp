#ifndef ENEMY_H
#define ENEMY_H

// 定数
#include "EnemyConfig.hpp"

#include "AnimationController.hpp"
#include "Sprite.hpp"

class Texture;
class Renderer;

// Enemyの向き管理
enum class EnemyDirection{
    Left, 
    Right
};

class Enemy{
private:
    // 画面の座標(左上が0, 0)
    double x, y;
    // 移動速度
    double speed;
    // 初期向きは右
    EnemyDirection dir = EnemyDirection::Right;
    AnimationController anim;
    Sprite sprite;

public:
    // 定数
    static inline constexpr int NUM_FRAMES = EnemyConfig::NUM_FRAMES;

    Enemy(Texture& tex, int fw, int fh);

    void update(double delta);
    void draw(Renderer& renderer);

};

#endif  //ENEMY_H