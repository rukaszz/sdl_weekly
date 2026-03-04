#ifndef PLAYERSTATESYSTEM_H
#define PLAYERSTATESYSTEM_H

#include "GameEventBuffer.hpp"
#include "Player.hpp"

class PlayerStateSystem{
private:
    Player& player;
public:
    explicit PlayerStateSystem(Player& p) : player(p){};
    ~PlayerStateSystem() = default;

    /**
     * @brief Playerに関係するイベントを消費する関数
     * 
     */
    void process(GameEventBuffer& events){
        events.consumeIf(
            // Pred
            [](const GameEvent& ev){return std::holds_alternative<CollectionItemEvent>(ev);}, 
            // Fn
            [&](const GameEvent& ev){
                // PredでヒットしたCollectionItemEventの要素をムーブ
                const auto& cie = std::get<CollectionItemEvent>(ev);
                // きのこ取得イベント
                if(cie.type == ItemType::Mushroom){
                    // 巨大化
                    player.setForm(PlayerForm::Super);
                }
                // きのこ取得イベント
                if(cie.type == ItemType::Coin){
                    // コイン取得でスコア加算
                    events.addScore(100);   // TODO：後でマジックナンバーを取り除く
                }
            }
        );
    }
};

#endif  // PLAYERSTATESYSTEM_H
