#include "ItemSystem.cpp"

#include <vector>
#include <variant>

#include "Item.hpp"
#include "GameEventBuffer.hpp"
#include "GameUtil.hpp"
#include "Player.hpp"

/**
 * @brief Construct a new Item System:: Item System object
 * 
 * @param items_ 
 */
explicit ItemSystem::ItemSystem(std::vector<Item>& items_)
    : items(items_)
{

}

/**
 * @brief ステージ読み込み時に呼ぶ関数
 * 
 */
void ItemSystem::onStageLoaded(){
    items.clear();
}

/**
 * @brief アイテムスポーンイベント
 * アイテムスポーンイベントがあればitemのvectorへ入れる
 * 
 * @param events 
 */
void ItemSystem::processSpawns(GameEventsBuffer& events){
    events.consumeIf(
        // Pred: SpawnItemEventのチェック
        [](const GameEvent& ev){ return std::holds_alternative<SpawnItemEvent>(ev);},
        // Fn: SpawnItemEventの消費
        [&](const GameEvent& ev){
            const auto& si = std::get<SpawnItemEvent>(ev);
            item.push_back(Item{si.type, si.x, si.y, true});    // 活性なのでactive
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
    // item vectorの探索
    for(auto& i : items){
        // 活性のみ処理
        if(!i.active){
            continue;
        }
        // アイテムの衝突判定用矩形取得
        const SDL_Rect ir = i.getCollisionRect();
        // 衝突していないのは処理しない
        if(!GameUtil::intersects(rt, ir)){
            continue;
        }
        // ここに到達=プレイヤーと接触しているので非活性に
        i.active = false;
        // アイテム取得イベント発生
        events.collectItem(i.type);
        // PlayerStateSystemではきのこなどプレイヤーの状態に関わるもののみ処理する
        // コインなどプレイヤーの状態に直接寄与しないものはここで処理する
        if(i.type == ItemType::Coin){
            // コイン取得でスコア加算
            events.addScore(100);   // TODO：後でマジックナンバーを取り除く
        }
    }
}

/**
 * @brief item vectorの整頓
 * 非活性なitemを数えてインデックスから省いて配列から取り除く
 * 
 */
void ItemSystem::cleanup(){
    // 活性を数える変数
    std::size_t out = 0;
    // 配列を1回舐める
    for(std::size_t i = 0; i < item.size(); ++i){
        // activeなitemのみoutで数える
        if(item[i].active){
            items[out++] = item[i];
        }
        // outに合わせてサイズ調整
        items.resize(out);
    }
}
