#include "Character.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include <algorithm>

/**
 * Characterクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 * Player/Enemyなどのオブジェクトが継承する
 */

Character::Character(
    double x_, double y_,
    double speed_,
    Direction dir_,
    Texture& tex,
    int frameW, int frameH,
    int maxFrames,
    double animInterval
)
    : x(x_)
    , y(y_)
    , speed(speed_)
    , dir(dir_)
    , anim(maxFrames, animInterval)
    , sprite(tex, frameW, frameH)
{
}

/**
 * @brief Destroy the Character:: Character object
 * 
 */
Character::~Character() = default;

/**
 * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
 * 
 * @param renderer: Rendererクラスのオブジェクト
 */
void Character::draw(Renderer& renderer){
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
    renderer.draw(sprite, dir == Direction::Left);
}

/**
 * @brief Spriteを読み取り専用で参照を返す
 * 
 * @return const Sprite& 
 */
const Sprite& Character::getSprite() const{
    return sprite;
}

/**
 * @brief Characterオブジェクトのx座標とy座標を変更する
 * 
 * @param coorX 
 * @param coorY 
 */
void Character::setPosition(double coorX, double coorY){
    x = coorX;
    y = coorY;
}

/**
 * @brief 画面外へはみ出さないように補正する処理
 * 
 * @param b 
 */
void Character::clampToBounds(const DrawBounds& b){
    x = std::clamp(x, 0.0, b.drawableWidth - sprite.getDrawWidth());
    y = std::clamp(y, 0.0, b.drawableHeight - sprite.getDrawHeight());
}
