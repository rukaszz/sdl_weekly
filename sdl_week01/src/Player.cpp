#include "Player.hpp"

/*
* Playerクラスは操作するオブジェクトを管理する
* 描画はしない
* SDLへの依存は最小限にする
*/

/**
 * @brief キーの入力に対して，差分を踏まえて更新する
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */
void Player::update(double delta, const Uint8* keystate){
    if(keystate[SDL_SCANCODE_LEFT]) {
        x -= speed * delta;
    }
    if(keystate[SDL_SCANCODE_RIGHT]) {
        x += speed * delta;
    }
    if(keystate[SDL_SCANCODE_UP]) {
        y -= speed * delta;
    }
    if(keystate[SDL_SCANCODE_DOWN]) {
        y += speed * delta;
    }
}