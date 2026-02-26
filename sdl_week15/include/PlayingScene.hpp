#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"
#include "EnemySensor.hpp"
#include "ProjectileSystem.hpp"
#include "EnemyAISystem.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

class Game;
class Renderer;
class TextTexture;
class Enemy;
struct InputState;
struct Block;
struct DrawBounds;

// /**
//  * @brief EnemyのAIが行動を判断するためのプレイヤー情報
//  * 
//  */
// struct PlayerInfo{
//     double center_X;    // プレイヤーの中心座標x
//     double center_Y;    // プレイヤーの中心座標y
// };

// /**
//  * @brief 敵の行動を決定するためのEnemy外部の情報
//  * gatherEnemySensorで使用する
//  * 
//  */
// struct EnemySensorContext{
//     const std::vector<Block>& blocks;
//     double worldWidth;
//     PlayerInfo playerInfo;
// };

/**
 * @brief ゲームのプレイシーン
 * プレイ中の更新・描画処理を担当し，付随する物理処理なども併せて処理する
 * 
 */
class PlayingScene : public Scene{
private:
    // 弾系オブジェクト管理用
    ProjectileSystem projectiles;
    // Enemyの行動判定用のSensor処理
    EnemyAISystem enemyAI;
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
    void detectCollision();
    void resolveBlockCollision();
    void resolveEnemyCollision(double prevFeet);
    void hasFallenToGameOver();
    bool checkBoundsforFireBalls(SDL_Rect fr, const double world_W, const double world_H);
};

#endif  // PLAYINGSCENE_H