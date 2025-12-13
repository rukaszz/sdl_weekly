// 定数
#include "GameConfig.hpp"
#include "EnemyConfig.hpp"

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Enemy.hpp"

#include <iostream>
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

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Enemy::getCollisionRect() const{
    return {(int)x+10, (int)y+10, sprite.getDrawWidth()-20, sprite.getDrawHeight()-20};
}


void Enemy::reset(std::mt19937& rd,
                  std::uniform_real_distribution<double>& dx,
                  std::uniform_real_distribution<double>& dy,
                  std::uniform_real_distribution<double>& ds)
{
    setPosition(dx(rd), dy(rd));
    setSpeed(ds(rd));
    dir = (rand()%2 == 0 ? Direction::Left : Direction::Right);
    anim.reset();
}
