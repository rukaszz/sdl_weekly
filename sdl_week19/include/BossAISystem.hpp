#ifndef BOSSAISYSTEM_H
#define BOSSAISYSTEM_H

#include <vector>

#include <SDL2/SDL.h>

#include "EnemySensor.hpp"

class BossEnemy;
class Player;

struct Block;
struct WorldInfo;
struct BossBattleState;

class BossAISystem{
private:
    BossEnemy& boss;
    const Player& player;
    const std::vector<Block>& blocks;
    const std::vector<SDL_Rect>& blockRects;
    const WorldInfo& world;
    const BossBattleState& bossBattle;

public:
    explicit BossAISystem(
        BossEnemy& boss_, 
        const Player& player_,
        const std::vector<Block>& blocks_,
        const std::vector<SDL_Rect>& blockRects_,
        const WorldInfo& world_,
        const BossBattleState& bossBattle_
    );
    ~BossAISystem() = default;

    // ステージロード時の初期化
    void onStageLoaded();
    // 更新関数
    void update(double delta);
};

#endif  // BOSSAISYSTEM_H
