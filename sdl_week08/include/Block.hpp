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

#endif  // BLOCK_H