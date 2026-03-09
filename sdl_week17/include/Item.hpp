#ifndef ITEM_H
#define ITEM_H

#include <utility>
#include <SDL2/SDL.h>
#include "GameEvent.hpp"

class Item{
private:
    ItemType type;      // アイテムの種類
    double x;           // 出現位置x
    double y;           // 出現位置y
    bool active = true; // 活性非活性
public:
    Item(ItemType type_, double x_, double y_)
        : type(type_), x(x_), y(y_), active(true) {}
    ~Item() = default;

    // getter/setter
    ItemType getItemType() const{
        return type;
    }
    double getItem_X() const{
        return x;
    }
    double getItem_Y() const{
        return y;
    }
    bool isActive() const{
        return active;
    }
    void deactivate(){
        active = false;
    }
    // 当たり判定用矩形を返す
    SDL_Rect getCollisionRect() const{
        // サイズに応じてw, hを決定
        const auto [w, h] = getSizeByType(type);
        return SDL_Rect{static_cast<int>(x), static_cast<int>(y), w, h};
    }
    // 仮のItemとしてSDL_RenderFillRect()を使用するので色を返す関数を定義
    SDL_Color getDebugColor() const{
        switch(type){
        case ItemType::Coin:
            return SDL_Color{255, 216, 0, 255}; // 黄色
        case ItemType::Mushroom:
            return SDL_Color{255, 64, 64, 255}; // 赤色
        }
        return SDL_Color{255, 255, 255, 255};
    }

private:
    /**
     * @brief Get the Size By ItemType object
     * 
     * @param type 
     * @return constexpr std::pair<int, int> 
     */
    static constexpr std::pair<int, int> getSizeByType(ItemType type){
        switch(type){
        case ItemType::Coin:
            return {24, 24};
        case ItemType::Mushroom:
            return {32, 32};
        }
        return {32, 32};
    }
};

#endif  // ITEM_H
