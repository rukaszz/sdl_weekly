#include "Bullet.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

#include <algorithm>

/**
 * Bulletクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 * Player/Enemyなどのオブジェクトが継承する
 */

Bullet::Bullet(
    double x_, double y_,
    double hv_,
    double vv_,
    Direction dir_,
    Texture& tex,
    int frameW, int frameH,
    int maxFrames,
    double animInterval
)
    : x(x_)
    , y(y_)
    , hv(hv_)
    , vv(vv_)
    , dir(dir_)
    , anim(maxFrames, animInterval)
    , sprite(tex, frameW, frameH)
{

}

/**
 * @brief Destroy the Bullet:: Bullet object
 * 
 */
Bullet::~Bullet() = default;

/**
 * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
 * カメラを考慮した版
 * 
 * @param renderer 
 * @param camera 
 */
void Bullet::draw(Renderer& renderer, Camera& camera){
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
    renderer.draw(sprite, camera, dir == Direction::Left);
}

/**
 * @brief Spriteを読み取り専用で参照を返す
 * 
 * @return const Sprite& 
 */
const Sprite& Bullet::getSprite() const{
    return sprite;
}

/**
 * @brief Bulletオブジェクトのx座標とy座標を変更する
 * 
 * @param coorX 
 * @param coorY 
 */
void Bullet::setPosition(double coorX, double coorY){
    x = coorX;
    y = coorY;
}

/**
 * @brief 画面外へはみ出さないように補正する処理
 * 
 * @param b 
 */
void Bullet::clampToBounds(const DrawBounds& b){
    x = std::clamp(x, 0.0, b.drawableWidth - sprite.getDrawWidth());
    y = std::clamp(y, 0.0, b.drawableHeight - sprite.getDrawHeight());
}
