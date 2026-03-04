#ifndef ITEM_H
#define ITEM_H

#include "GameEvent.hpp"
#include <SDL2/SDL.h>

class Item{
private:

public:
    ItemType type;      // アイテムの種類
    double x;           // 出現位置x
    double y;           // 出現位置y
    bool active = true; // 活性非活性

    Item() = default;
    ~Item() = default;

    SDL_Rect getCollisionRect() const{
        // TODO：サイズは暫定．後でマジックナンバーを取り除く
        const int w = 32;
        const int h = 32;
        return SDL_Rect{static_cast<int>(x), static_cast<int>(y), w, h};
    }
};

#endif  // ITEM_H
