#include "ItemSystem.hpp"

#include <vector>
#include <variant>
#include <algorithm>

#include "Renderer.hpp"
#include "Camera.hpp"
#include "Item.hpp"
#include "GameEventBuffer.hpp"
#include "GameUtil.hpp"
#include "Player.hpp"

/**
 * @brief Construct a new Item System:: Item System object
 * 
 * @param items_ 
 */
ItemSystem::ItemSystem(std::vector<Item>& items_)
    : items(items_)
{

}

/**
 * @brief ステージ読み込み時に呼ぶ関数
 * 
 */
void ItemSystem::onStageLoaded(){
    // 現状は内部キャッシュを持たないので何もしない
}

/**
 * @brief アイテムスポーンイベント
 * アイテムスポーンイベントがあればitemのvectorへ入れる
 * 
 * @param events 
 */
void ItemSystem::processSpawn(GameEventBuffer& events){
    events.consumeIf(
        // Pred: SpawnItemEventのチェック
        [](const GameEvent& ev){ return std::holds_alternative<SpawnItemEvent>(ev);},
        // Fn: SpawnItemEventの消費
        [&](const GameEvent& ev){
            const auto& si = std::get<SpawnItemEvent>(ev);
            // items.push_back(Item(si.x, si.y, si.type));    
            items.emplace_back(si.x, si.y, si.type);    // 活性なのでactive
        }
    );
}

/**
 * @brief アイテムを「取得」するためのプレイヤーとアイテムの接触判定処理
 * 一部アイテム取得時のイベントもここで呼ぶ
 * 
 * @param player 
 * @param events 
 */
void ItemSystem::resolvePlayerCollision(Player& player, GameEventBuffer& events){
    // playerの衝突判定用矩形の取得
    const SDL_Rect pr = player.getCollisionRect();
    // items vectorの探索
    for(auto& item : items){
        // 活性のみ処理
        if(!item.isActive()){
            continue;
        }
        // アイテムの衝突判定用矩形取得
        const SDL_Rect ir = item.getCollisionRect();
        // 衝突していないのは処理しない
        if(!GameUtil::intersects(pr, ir)){
            continue;
        }
        // ここに到達=プレイヤーと接触しているので非活性に
        item.deactivate();
        // アイテム取得イベント発生
        events.collectItem(item.getItemType());
        // PlayerStateSystemではきのこなどプレイヤーの状態に関わるもののみ処理する
        // コインなどプレイヤーの状態に直接寄与しないものはここで処理する
        if(item.getItemType() == ItemType::Coin){
            // コイン取得でスコア加算
            events.addScore(100);   // TODO：後でマジックナンバーを取り除く
        }
    }
}

/**
 * @brief items vectorの整頓
 * 非活性なitemを数えてインデックスから省いて配列から取り除く
 * 
 */
void ItemSystem::cleanup(){
    auto it = std::remove_if(
        items.begin(), 
        items.end(), 
        [](const Item& item){
            return !item.isActive();
        }
    );
    items.erase(it, items.end());
}

void ItemSystem::render(Renderer& renderer, Camera& camera){
    for(const auto& item : items){
        if(!item.isActive()){
            continue;
        }
        renderer.drawRect(item.getCollisionRect(), item.getDebugColor(), camera);
    }
}
