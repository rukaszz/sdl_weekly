#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "ItemRenderer.hpp"
#include "BlockRenderer.hpp"

#include "ProjectileSystem.hpp"
#include "EnemyAISystem.hpp"
#include "BossAISystem.hpp"
#include "CollisionSystem.hpp"
#include "BlockSystem.hpp"
#include "ItemSystem.hpp"

#include "GameEventBuffer.hpp"
#include "PlayerStateSystem.hpp"
#include "BossBattleState.hpp"

class Game;
class Renderer;
class TextTexture;
class Enemy;
struct InputState;
struct Block;
struct DrawBounds;

/**
 * @brief プレイ中/ポーズ中
 * 
 */
enum class RunState{
    Running, 
    Paused, 
};

/**
 * @brief ゲームのプレイシーン
 * プレイ中の更新・描画処理を担当し，付随する物理処理なども併せて処理する
 * 
 */
class PlayingScene : public Scene{
private:
    // 生存時間加算用変数
    double survivalScoreRemainder = 0.0;
    // ポーズ判定用変数
    RunState runState = RunState::Running;
    // 弾系オブジェクト管理用
    ProjectileSystem projectiles;
    // Enemyの行動判定用のSensor処理
    EnemyAISystem enemyAI;
    // Bossの行動判定用Sensor処理
    BossAISystem bossAI;
    // 衝突処理管理用
    CollisionSystem collision;
    // イベント管理用
    // GameEventBuffer events;
    // ブロック管理用
    BlockSystem blockSystem;
    // アイテム管理用
    ItemSystem items;
    // Playerの状態管理用
    PlayerStateSystem playerState;
    // ボス演出系
    BossBattleState bossBattle;
    // Itemレンダリング用
    ItemRenderer itemRenderer;
    // Blockレンダリング用
    BlockRenderer blockRenderer;
    // ポーズ時の文字列
    std::unique_ptr<TextTexture> pauseTitleText;
    std::unique_ptr<TextTexture> gameResumeText;
    std::unique_ptr<TextTexture> backToTitleText;
    // デバッグ表示用のテキストテクスチャ
    std::unique_ptr<TextTexture> debugText;
    
public:
    PlayingScene(SceneControl& sc, GameContext& gc);
    virtual ~PlayingScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    // update内部で呼ばれる処理の分割
    void handlePlayingInput(const InputState& is);
    void updateScore(double delta);
    void updateEntities(double delta, DrawBounds b);
    void updateCamera();
    // ポーズ関係の処理
    void renderPauseOverlay();
    // ボス関係※現状1体なのでSystemに分割しない
    // ステージ入場時に1回呼ぶ初期化関数
    void initBossBattle();
    // トリガー監視関数
    void updateBossBattleTrigger();
    // think()/update()をする関数
    void updateBoss(double delta, DrawBounds bounds);
    // ボスがDying/Deadになったときの関数
    void updateBossBattleResult();
    // ボスのHPバー描画関数
    void renderBossHpBar();
};

#endif  // PLAYINGSCENE_H
