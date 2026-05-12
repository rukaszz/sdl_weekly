#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "ItemRenderer.hpp"
#include "BlockRenderer.hpp"
#include "BackgroundRenderer.hpp"

#include "ProjectileSystem.hpp"
#include "EnemyAISystem.hpp"
#include "BossBattleSystem.hpp"
#include "CollisionSystem.hpp"
#include "BlockSystem.hpp"
#include "ItemSystem.hpp"
#include "CameraShakeController.hpp"

#include "GameEventBuffer.hpp"
#include "PlayerStateSystem.hpp"
#include "BackgroundId.hpp"

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
    // 背景レンダラ
    BackgroundRenderer bgRenderer;
    // 弾系オブジェクト管理用
    ProjectileSystem projectiles;
    // Enemyの行動判定用のSensor処理
    EnemyAISystem enemyAI;
    // ボス戦管理
    BossBattleSystem bossBattleSystem;
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
    // Itemレンダリング用
    ItemRenderer itemRenderer;
    // Blockレンダリング用
    BlockRenderer blockRenderer;
    // ポーズ時の文字列
    std::unique_ptr<TextTexture> pauseTitleText;
    std::unique_ptr<TextTexture> gameResumeText;
    std::unique_ptr<TextTexture> backToTitleText;
    // 画面シェイク
    CameraShakeController cameraShake;
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
    // 画面シェイクイベント消費用
    void consumeShakeEffectEvents();
    // update内部で呼ばれる処理の分割
    void handlePlayingInput(const InputState& is);
    void updateScore(double delta);
    void updateEntities(double delta, DrawBounds b);
    void updateCamera();
    // ポーズ関係の処理
    void renderPauseOverlay();
    // 背景読み込み処理
    void loadBackground();
};

#endif  // PLAYINGSCENE_H
