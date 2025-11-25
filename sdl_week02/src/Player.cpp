#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
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
    : x(100), y(100), speed(200.0), sprite(tex)
{

}

/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分
 * @param keystate: キーの入力状態
 */
void Player::update(double delta, const Uint8* keystate, SDL_Point drawableSize){
    if(keystate[SDL_SCANCODE_LEFT]){
        x -= speed * delta;
    }
    if(keystate[SDL_SCANCODE_RIGHT]){
        x += speed * delta;
    }
    if(keystate[SDL_SCANCODE_UP]){
        y -= speed * delta;
    }
    if(keystate[SDL_SCANCODE_DOWN]){
        y += speed * delta;
    }

    // x = std::clamp(x, 0.0, drawableSize.x - static_cast<double>(Player::SIZE));
    // y = std::clamp(y, 0.0, drawableSize.y - static_cast<double>(Player::SIZE));
}

/**
 * @brief スプライトオブジェクトが持っている画像をレンダラーへ描画する
 * 
 * @param renderer: Rendererクラスのオブジェクト
 */
void Player::draw(Renderer& renderer){
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
    sprite.draw(renderer);
}