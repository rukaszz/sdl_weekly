#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"

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
        100, 250,                           // 初期座標
        200.0,                              // speed
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
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */
void Player::update(double delta, DrawBounds bounds){
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    // 移動中以外はfalse
    bool moving = false;
    
    if(keystate[SDL_SCANCODE_LEFT]){
        x -= speed * delta;
        dir = Direction::Left;
        moving = true;
    }
    if(keystate[SDL_SCANCODE_RIGHT]){
        x += speed * delta;
        dir = Direction::Right;
        moving = true;
    }
    if(keystate[SDL_SCANCODE_UP]){
        y -= speed * delta;
        moving = true;
    }
    if(keystate[SDL_SCANCODE_DOWN]){
        y += speed * delta;
        moving = true;
    }

    clampToBounds(bounds);

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

