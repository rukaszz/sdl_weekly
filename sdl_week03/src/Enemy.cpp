#include "Renderer.hpp"
#include "Texture.hpp"
#include "Enemy.hpp"

#include <algorithm>
#include <random>

/**
 * Enemyクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 */

/**
 * @brief Construct a new Enemy:: Enemy object
 * 
 * @param tex 
 */
Enemy::Enemy(Texture& tex)
    : Character(
        0, 0,                               // 初期座標
        100.0,                              // speed
        Direction::Right,                   // dir
        tex,                                // texture
        EnemyConfig::FRAME_W, 
        EnemyConfig::FRAME_H, 
        EnemyConfig::NUM_FRAMES,            // maxFrames
        0.1                                 // animInterval
    )
    
{
    sprite.setFrame(0);
    // 乱数生成器
    static std::random_device rd;  // 非決定的なシード
    static std::mt19937 gen(rd());
    // 乱数：位置(y軸のみ)
    std::uniform_int_distribution<> posY(50, 700);
    y = posY(gen);
    // 乱数：speed
    std::uniform_real_distribution<> dist(10, 90);
    speed = speed - dist(gen);
}

/**
 * @brief Enemyの動きを記述する(※現状は左右に動くだけ)
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分 
 * @param drawableSize: 描画可能範囲 
 */
void Enemy::update(double delta, DrawBounds bounds){
    double leftBound = 0.0;
    double rightBound = bounds.drawableWidth - static_cast<double>(sprite.getDrawWidth());

    if(dir == Direction::Right){
        x += speed * delta;
    } else {
        x -= speed * delta;
    }

    if(x < leftBound) {
        x = leftBound;
        dir = Direction::Right;
    }
    if(x > rightBound) {
        x = rightBound;
        dir = Direction::Left;
    }
    // アニメーション処理
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

