#ifndef ITEM_H
#define ITEM_H

#include <utility>
#include <SDL2/SDL.h>
#include "GameEvent.hpp"

class Item{
private:
    double x;           // 出現位置x
    double y;           // 出現位置y
    ItemType type;      // アイテムの種類
    bool active = true; // 活性非活性
public:
    Item(double x_, double y_, ItemType type_)
        : x(x_), y(y_), type(type_), active(true) {}
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
    // 描画用矩形を返す
    SDL_Rect getDrawRect() const{
        // サイズに応じてw, hを決定
        const auto [w, h] = getSizeByType(type);
        // 当たり判定用矩形の中心を鑑みてオフセットを設定
        const SDL_Rect colRect = getCollisionRect();
        const int cx = colRect.x + (colRect.w / 2);
        const int cy = colRect.y + (colRect.h / 2); 
        return SDL_Rect{cx - w/2, cy - h/2, w, h};
    }
    // 仮のItemとしてSDL_RenderFillRect()を使用するので色を返す関数を定義
    SDL_Color getDebugColor() const{
        switch(type){
        case ItemType::Coin:
            return SDL_Color{255, 216, 0, 255}; // 黄色
        case ItemType::Mushroom:
            return SDL_Color{255, 64, 64, 255}; // 赤色
        case ItemType::FireFlower:
            return SDL_Color{16, 160, 80, 255}; // 黄緑色
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
        case ItemType::FireFlower:
            return {32, 32};
        }
        return {32, 32};
    }
};

#endif  // ITEM_H
