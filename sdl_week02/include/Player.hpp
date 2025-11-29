#ifndef PLAYER_H
#define PLAYER_H

// 定数
#include "PlayerConfig.hpp"

#include "Sprite.hpp"

#include <SDL2/SDL.h>

class Sprite;
class Texture;
class Renderer;

class Player{
private:
    // 画面の座標(左上が0, 0)
    double x, y;
    // 移動速度
    double speed;
    // アニメーション描画用変数
    int frame = 0;
    double frameTimer = 0.0;
    const double frameInterval = 0.1;

    Sprite sprite;

public:
    // 定数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;

    Player(Texture& tex, int fw, int fh);

    void update(double delta, const Uint8* keystate, SDL_Point drawableSize);
    void draw(Renderer& renderer);

};

#endif  // PLAYER_H