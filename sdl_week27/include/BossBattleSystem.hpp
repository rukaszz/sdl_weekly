#ifndef BOSSBATTLESYSTEM_H
#define BOSSBATTLESYSTEM_H

#include <vector>

#include <SDL2/SDL.h>

#include "BossAISystem.hpp"
#include "BossBattleState.hpp"

class BossEnemy;
class Player;
class Renderer;
class MusicSystem;
class IGameEvents;
struct Block;
struct WorldInfo;
struct StageDefinition;
struct InputState;
struct DrawBounds;
struct Camera;

/**
 * @brief ボス戦に関係する情報を管理するクラス
 * Player-Bossの接触判定などはここが実施する
 * 
 */
class BossBattleSystem{
private: 
    // 初期化順は宣言順であることが言語で保証されているので，BossBattleState→BossAISystemの順で宣言
    BossBattleState state;  // BossAISystemより先に宣言しておく
    BossEnemy& boss;
    BossAISystem bossAI;    // state& を渡す
    const Player& player;
    const std::vector<Block>& blocks;

public:
    // コンストラクタ/デストラクタ
    BossBattleSystem(
        BossEnemy& boss_,
        const Player& player_,
        const std::vector<Block>& blocks_,
        const std::vector<SDL_Rect>& blockRects_,
        const WorldInfo& world_
    );
    ~BossBattleSystem() = default;
    // ステージ入場時の初期化関係
    void init(const StageDefinition& def);
    void onStageLoaded();
    
    // トリガー判定関数
    void updateTrigger(MusicSystem& music, IGameEvents& events);
    // AI更新関数
    void updateAI(double delta);
    // ボス更新
    void updateBoss(double delta, const DrawBounds& bounds, const InputState& is);
    // ボス戦の結果判定
    void checkBattleResult(IGameEvents& events);
    // Player vs Bossの接触判定(playerがダメージを受ける可能性もあるので非const)
    void resolveBossPlayerCollision(Player& player, IGameEvents& events);
    // 描画
    void renderHpBar(Renderer& renderer, const Camera& camera) const;

    // ボス戦時のカメラ制限で使用するためのgetter
    bool isActive() const noexcept{
        return state.isActive();
    }
    bool isWaiting() const noexcept{
        return state.isWaiting();
    }
    bool hasBoss() const noexcept{
        return state.hasBoss();
    }
    double getCameraMin_X() const noexcept{
        return state.cameraMin_X;
    }
    double getCameraMax_X() const noexcept{
        return state.cameraMax_X;
    }
};

#endif  // BOSSBATTLESYSTEM_H
