#ifndef BLOCK_H
#define BLOCK_H

#include <SDL2/SDL.h>
#include <cstdint>

enum class BlockType : std::uint8_t {
    Standable,      // 乗れるブロック
    DropThrough,    // 下へ抜けられるブロック
    Damage,         // 触れるとダメージを受ける床
    Clear,          // 触れるとステージクリア
    Question,       // アイテムが入っているブロック
    UsedQuestion,   // アイテムを取り出した後のブロック
    Breakable,      // 破壊可能なブロック
    Empty,          // 表示・処理をしない状態
};

struct Block{
    double x; 
    double y; 
    double w; 
    double h;
    BlockType type;
};

#endif  // BLOCK_H