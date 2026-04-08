#include "BlockSystem.hpp"

#include <vector>
#include <variant>

#include "Block.hpp"
#include "GameEventBuffer.hpp"
#include "Player.hpp"

/**
 * @brief Construct a new Block System:: Block System object
 * 
 * @param blocks_ 
 * @param player_ 
 */
BlockSystem::BlockSystem(std::vector<Block>& blocks_, Player& player_) 
        : blocks(blocks_), player(player_)
{

}

/**
 * @brief ブロックを頭突きした際イベント処理を実施する
 * 
 * @param events 
 */
void BlockSystem::process(GameEventBuffer& events){
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
                const double spawn_X = b.x + b.w * 0.5 - 16;
                const double spawn_Y = b.y - 32;
                switch(b.reward){
                case BlockRewardType::Coin:
                    events.spawnItem(ItemType::Coin, spawn_X, spawn_Y);
                    events.addScore(100);
                    break;
                case BlockRewardType::Mushroom:{
                    const ItemType spawnType = (player.getForm() == PlayerForm::Small)
                                                ? ItemType::Mushroom : ItemType::FireFlower;
                    events.spawnItem(spawnType, spawn_X, spawn_Y);
                    break;
                }
                case BlockRewardType::FireFlower:
                    events.spawnItem(ItemType::FireFlower, spawn_X, spawn_Y);
                    break;
                case BlockRewardType::None:
                    break;
                }
                // 内部のアイテムも空にする
                b.reward = BlockRewardType::None;
                return;
            }
            // NOTE：Breakableの場合は消すので，RectCacheとの整合性を取る必要あり
            // 種類変更はindex維持，破壊はactive=falseにして内部で保持
            // 内部保持はタイミングを見て消す？
            if(b.type == BlockType::Breakable){
                if(player.getForm() != PlayerForm::Small){
                    b.type = BlockType::Empty;   // 破壊されるので消す的な状態へ
                    events.addScore(50);
                }
                return;
            }
        }
    );
}
