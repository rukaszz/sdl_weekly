#ifndef BLOCKSYSTEM_H
#define BLOCKSYSTEM_H

#include <vector>
#include <variant>

#include "Block.hpp"
#include "GameEventBuffer.hpp"

class BlockSystem{
private:
    std::vector<Block>& blocks;

public:
    explicit BlockSystem(std::vector<Block>& blocks_) : blocks(blocks_){}

    void process(GameEventBuffer& events){
        events.consumeIf(
            // Pred: BlockHitEventの取り出し
            [](const GameEvent& ev){ return std::holds_alternative<BlockHitEvent>(ev); }, 
            // Fn: BlockHitEventの消費
            [&](const GameEvent& ev){
                const auto& bhe = std::get<BlockHitEvent>(ev);
                // イベント対象のブロックのインデックスがblocksのサイズを超えていたら(処理対象がないので)何もしない
                if(bhe.blockIndex >= blocks.size()){
                    return;
                }
                // blockIndexに対応するブロックの情報を取得
                auto& b = blocks[bhe.blockIndex];
                // Questionブロックならアイテムを出すイベントが発生する
                if(b.type == BlockType::Question){
                    // ブロックを使用済みに
                    b.type = BlockType::UsedQuestion;
                    // アイテムを出す(中央より若干上)
                    const double spawn_X = (b.x + b.w) / 2 - 16;
                    const double spawn_Y = b.y - 32;
                    events.spawnItem(ItemType::Coin, spawn_X, spawn_Y);
                    // Questionブロックヒット時にスコア加算(要検討)
                    events.addScore(50);
                }
            }
        );
    }
};

#endif  // BLOCKSYSTEM_H
