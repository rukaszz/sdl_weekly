#ifndef ITEM_H
#define ITEM_H

#include <utility>

#include "GameEvent.hpp"
#include <SDL2/SDL.h>

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
    SDL_Rect getCollisionRect() const{
        // サイズに応じてw, hを決定
        const auto [w, h] = getSizeByType(type);
        return SDL_Rect{static_cast<int>(x), static_cast<int>(y), w, h};
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
