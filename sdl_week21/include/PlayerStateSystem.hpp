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
            [](const GameEvent& ev){return std::holds_alternative<CollectItemEvent>(ev);}, 
            // Fn
            [&](const GameEvent& ev){
                // PredでヒットしたCollectItemEventの要素をムーブ
                const auto& cie = std::get<CollectItemEvent>(ev);
                // きのこ取得イベント
                if(cie.type == ItemType::Mushroom){
                    // 巨大化
                    player.setForm(PlayerForm::Super);
                }
                // ファイアフラワー取得イベント
                if(cie.type == ItemType::FireFlower){
                    // ファイア状態
                    player.setForm(PlayerForm::Fire);
                }
            }
        );
    }
};

#endif  // PLAYERSTATESYSTEM_H
