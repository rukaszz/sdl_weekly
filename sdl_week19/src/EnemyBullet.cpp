#include "EnemyBullet.hpp"

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Block.hpp"

#include "EnemyBulletConfig.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

/**
 * @brief Construct a new Enemy Bullet:: Enemy Bullet object
 * 
 * @param x 
 * @param y 
 * @param dir 
 * @param tex 
 */
EnemyBullet::EnemyBullet(double x, double y, Direction dir, Texture& tex)
    : Bullet(
        x, y,   // x, y
        (dir == Direction::Right ? EnemyBulletConfig::SPEED_X : -EnemyBulletConfig::SPEED_X),     // hv
        0.0,    // vv
        dir,    //dir
        tex,    // tex
        EnemyBulletConfig::FRAME_W,         // frameW 
        EnemyBulletConfig::FRAME_H,         // frameH
        EnemyBulletConfig::NUM_FRAMES,      // maxFrames    
        EnemyBulletConfig::ANIM_INTERVAL    // animInterval
    )
{
    sprite.setFrame(0);
}

/**
 * @brief ファイアボールの挙動を設定
 * 現状は床(上から下)とのバウンドがメイン
 * 
 * @param delta 
 * @param blocks 
 */
void EnemyBullet::update(double delta, const std::vector<Block>& /*blocks*/){
    // 死んだ弾は何もしない
    if(!active){
        return;
    }
    // 1. 等速直線運動
    x += hv * delta;

    // 2. アニメーション更新
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief 当たり判定用の矩形を返却する関数
 * 
 * @return SDL_Rect 
 */
SDL_Rect EnemyBullet::getCollisionRect() const{
    return {
        static_cast<int>(x),
        static_cast<int>(y),
        sprite.getDrawWidth(),
        sprite.getDrawHeight()
    };
}
