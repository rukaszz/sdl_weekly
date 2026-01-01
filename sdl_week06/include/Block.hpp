#ifndef BLOCK_H
#define BLOCK_H

#include <SDL2/SDL.h>

enum class BlockType{
    Standable,      // 乗れるブロック
    DropThrough,    // 下へ抜けられるブロック
    Damage          // 触れるとダメージを受ける床
};

struct Block{
    double x; 
    double y; 
    double w; 
    double h;
    BlockType type;
};

/**
 * @brief Blockの値をSDL_Rectへ変換する関数
 * 
 * @param b 
 * @return SDL_Rect 
 */
inline SDL_Rect blockToRect(const Block& b){
    return SDL_Rect{
        static_cast<int>(b.x),
        static_cast<int>(b.y),
        static_cast<int>(b.w),
        static_cast<int>(b.h)
    };
}

#endif  // BLOCK_H