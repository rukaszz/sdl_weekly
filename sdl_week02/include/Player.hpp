#ifndef PLAYER_H
#define PLAYER_H
#include "Sprite.hpp"
#include <SDL2/SDL.h>

class Sprite;
class Texture;
class Renderer;

class Player{
private:
    // 画面の座標(左上が0, 0)
    double x, y;
    double speed;
    Sprite sprite;

public:
    // 定数
    static inline constexpr int SIZE = 50;

    Player(Texture& tex);

    void update(double delta, const Uint8* keystate, SDL_Point drawableSize);
    void draw(Renderer& renderer);

    // 矩形を返すオブジェクト
    SDL_Rect getRect() const{
        return SDL_Rect{(int)x, (int)y, SIZE, SIZE};
    }
};

#endif  // PLAYER_H