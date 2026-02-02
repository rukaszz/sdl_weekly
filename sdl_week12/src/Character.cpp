#include "Character.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

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
    , speed(speed_)
    , hv(hv_)
    , vv(vv_)
    , dir(dir_)
    , anim(maxFrames, animInterval)
    , sprite(tex, frameW, frameH)
{
    // 接触判定用座標の初期化
    prevTopPhysics = 0.0;
    prevFeetPhysics = 0.0;
    prevFeetCollision = 0.0;
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
// void Character::draw(Renderer& renderer){
//     sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
//     renderer.draw(sprite, dir == Direction::Left);
// }

/**
 * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
 * カメラを考慮した版
 * 
 * @param renderer 
 * @param camera 
 */
void Character::draw(Renderer& renderer, Camera& camera){
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
    renderer.draw(sprite, camera, dir == Direction::Left);
}

/**
 * @brief エンティティのx軸の中心位置を返す
 * 主にカメラなどの処理で使う
 * 
 * @return double 
 */
double Character::getEntityCenter_X(){
    return x + (sprite.getDrawWidth() / 2.0);
}

/**
 * @brief エンティティのy軸の中心位置を返す
 * 主にカメラなどの処理で使う
 * 
 * @return double 
 */
double Character::getEntityCenter_Y(){
    return y + (sprite.getDrawHeight() / 2.0);
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
