#include "Player.hpp"
#include <algorithm>

/*
* Playerクラスは操作するオブジェクトを管理する
* 描画はしない
* SDLへの依存は最小限にする
*/

/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */
void Player::update(double delta, const Uint8* keystate, SDL_Point drawableSize){
    if(keystate[SDL_SCANCODE_LEFT]) {
        x -= speed * delta;
        x = std::clamp(x, 0.0, drawableSize.x - Player::SIZE);
    }
    if(keystate[SDL_SCANCODE_RIGHT]) {
        x += speed * delta;
        x = std::clamp(x, 0.0, drawableSize.x - Player::SIZE);
    }
    if(keystate[SDL_SCANCODE_UP]) {
        y -= speed * delta;
        y = std::clamp(y, 0.0, drawableSize.y - Player::SIZE);
    }
    if(keystate[SDL_SCANCODE_DOWN]) {
        y += speed * delta;
        y = std::clamp(y, 0.0, drawableSize.y - Player::SIZE);
    }
}