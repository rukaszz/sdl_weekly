#include "Renderer.hpp"
#include "Texture.hpp"
#include "Enemy.hpp"

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

// /**
//  * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
//  * 
//  * @param renderer: Rendererクラスのオブジェクト
//  */
// void Enemy::draw(Renderer& renderer){
//     sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
//     renderer.draw(sprite, dir == Direction::Left);
// }

// /**
//  * @brief Enemyのスプライトを返す関数
//  * 
//  * @return const Sprite& 
//  */
// const Sprite& Enemy::getSprite() const {
//     return sprite; 
// }


// /**
//  * @brief Enemyオブジェクトのx座標とy座標を変更する
//  * 
//  * @param coorX 
//  * @param coorY 
//  */
// void Enemy::setPosition(int coorX, int coorY){
//     x = coorX;
//     y = coorY;
// }

// /**
//  * @brief 画面外へはみ出さないように補正する処理
//  * 
//  * @param b 
//  */
// void Character::clampToBounds(const DrawBounds& b){
//     x = std::clamp(x, 0.0, b.drawableWidth - sprite.getDrawWidth());
//     y = std::clamp(y, 0.0, b.drawableHeight - sprite.getDrawHeight());
// }

