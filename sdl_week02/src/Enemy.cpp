#include "Enemy.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"


#include <SDL2/SDL.h>
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
    , anim(10, 0.1)
    , sprite(tex, fw, fh)
{
    sprite.setFrame(0);
}

/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */
void Enemy::update(double delta){
    const double leftBound = 50;
    const double rightBound = 350;

    if(dir == Direction::Right)
        x += speed * delta;
    else
        x -= speed * delta;

    if(x < leftBound)  dir = Direction::Right;
    if(x > rightBound) dir = Direction::Left;
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
    renderer.drawSpriteFlip(sprite, dir == Direction::Left);
}