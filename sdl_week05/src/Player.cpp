#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Input.hpp"

#include "GameConfig.hpp"
#include "PlayerConfig.hpp"

#include <SDL2/SDL.h>
#include <algorithm>

/**
 * Playerクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 */

/**
 * @brief Construct a new Player:: Player object
 * 
 * @param tex 
 */
Player::Player(Texture& tex)
    : Character(
        PlayerConfig::POS_X, 
        PlayerConfig::POS_Y,                // 初期座標
        200.0,                              // speed
        0.0,                                // 垂直速度
        Direction::Right,                   // dir
        tex,                                // texture
        PlayerConfig::FRAME_W, 
        PlayerConfig::FRAME_H,
        PlayerConfig::NUM_FRAMES,           // maxFrames
        0.1                                 // animInterval
    )
{
    sprite.setFrame(0);
}

/**
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */


/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * ジャンプを実装するので上下移動はなくなる
 * 
 * @param delta: 差分 
 * @param input: Inputクラスの状態=キー入力 
 * @param bounds: 描画可能範囲 
 */
void Player::update(double delta, const InputState& input, DrawBounds bounds){
    // 水平移動用変数
    double moveDir = 0.0;
    // 移動中以外はfalse
    bool moving = false;
    
    if(input.pressed[(int)Action::MoveLeft]){
        // x -= speed * delta;
        moveDir -= 1.0;
        dir = Direction::Left;
        moving = true;
    }
    if(input.pressed[(int)Action::MoveRight]){
        // x += speed * delta;
        moveDir += 1.0;
        dir = Direction::Right;
        moving = true;
    }
    // Jump
    if(onGround && input.justPressed[(int)Action::Jump]){
        vv = -PlayerConfig::JUMP_VELOCITY;
        onGround = false;
    }
    /*
    if(input.pressed[(int)Action::MoveUp]){
        y -= speed * delta;
        moving = true;
    }
    if(input.pressed[(int)Action::MoveDown]){
        y += speed * delta;
        moving = true;
    }
    */

    // 水平方向移動
    x += moveDir * speed * delta;
    // 垂直方向移動
    vv += PlayerConfig::PLAYER_GRAVITY * delta;
    y += vv * delta;

    // clampToBounds(bounds);
    clampToGround(bounds);
    clampHorizontalPosition(bounds);

    // アニメーション処理
    if(!moving){
        anim.reset();
        sprite.setFrame(anim.getFrame());
        return;
    }
    // フレームを動かして描画
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Player::getCollisionRect() const{
    return {(int)x+10, (int)y+10, sprite.getDrawWidth()-20, sprite.getDrawHeight()-20};
}

/**
 * @brief プレイヤーの位置を初期位置へ移動させる
 * 
 */
void Player::reset(){
    setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    anim.reset();
}

/**
 * @brief 床との接地管理用のクランプ関数
 * 
 * @param bounds 
 */
void Player::clampToGround(const DrawBounds& bounds){
    // 床の位置
    double floor_y = bounds.drawableHeight - sprite.getDrawHeight();

    if(y >= floor_y){
        y = floor_y;
        vv = 0.0;
        onGround = true;
    } else {
        onGround = false;
    }
}

/**
 * @brief x軸との接地管理用のクランプ関数
 * 
 * @param bounds 
 */
void Player::clampHorizontalPosition(const DrawBounds& bounds){
    x = std::clamp(x, 0.0, bounds.drawableWidth - sprite.getDrawWidth());
}

