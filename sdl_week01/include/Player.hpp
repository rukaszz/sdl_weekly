#ifndef PLAYER_H
#define PLAYER_H
#include <SDL2/SDL.h>

class Player{
private:
    // 画面の座標(左上が0, 0)
    double x, y;
    double speed;

public:
    // 定数
    static inline constexpr double SIZE = 50.0;

    Player(double startX = 100, double startY = 100)
        : x(startX), y(startY), speed(200.0) {}

    void update(double delta, const Uint8* keystate, SDL_Point windowSize);

    // 矩形を返すオブジェクト
    SDL_Rect getRect() const{
        return SDL_Rect{(int)x, (int)y, (int)SIZE, (int)SIZE};
    }
};


#endif  // PLAYER_H