#ifndef BLOCK_H
#define BLOCK_H

#include <SDL2/SDL.h>
#include <cstdint>

/**
 * @brief ステージに出現するブロックの種類に関する列挙型
 * 
 */
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

/**
 * @brief Questionブロックを叩いた際の出現(報酬)アイテムに関する列挙型
 * 
 */
enum class BlockRewardType : std::uint8_t{
    None,       // 何もない
    Coin,       // コイン
    Mushroom,   // きのこ
    FireFlower, // ファイアフラワー
};

/**
 * @brief ブロックのデータ
 * 
 */
struct Block{
    double x; 
    double y; 
    double w; 
    double h;
    BlockType type;
    BlockRewardType reward = BlockRewardType::None;
};

#endif  // BLOCK_H
