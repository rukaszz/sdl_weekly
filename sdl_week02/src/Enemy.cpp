// 定数
#include "EnemyConfig.hpp"

#include "Enemy.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include <algorithm>

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
Enemy::Enemy(Texture& tex, int fw, int fh)
    : x(0)
    , y(0)
    , speed(100.0)
    , anim(EnemyConfig::NUM_FRAMES, 0.1)
    , sprite(tex, fw, fh)
{
    sprite.setFrame(0);
}

/**
 * @brief Enemyの動きを記述する(※現状は左右に動くだけ)
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分
 */
void Enemy::update(double delta){
    const double leftBound = 50;
    const double rightBound = 300;

    if(dir == EnemyDirection::Right)
        x += speed * delta;
    else
        x -= speed * delta;

    if(x < leftBound)  {
        x = leftBound;
        dir = EnemyDirection::Right;
    }
    if(x > rightBound) {
        x = rightBound;
        dir = EnemyDirection::Left;
    }
    // アニメーション処理
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
 * 
 * @param renderer: Rendererクラスのオブジェクト
 */
void Enemy::draw(Renderer& renderer){
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
    renderer.drawSpriteFlip(sprite, dir == EnemyDirection::Left);
}