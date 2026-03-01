#include "SceneControl.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>

#include "Block.hpp"
#include "BlockLevelLoader.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "WalkerEnemy.hpp"
#include "ChaserEnemy.hpp"
#include "JumperEnemy.hpp"
#include "TurretEnemy.hpp"
#include "FireBall.hpp"
#include "EnemyBullet.hpp"
#include "GameEventBuffer.hpp"

#include "WorldInfo.hpp"
#include "GameScene.hpp"
#include "GameConfig.hpp"
#include "StageConfig.hpp"
#include "GameContext.hpp"
#include "GameUtil.hpp"

// SceneControl内でのみ使えるヘルパ関数
namespace{
    /**
     * @brief シーンの優先順位を返す関数
     * 優先順位：GameOver > Clear > Title > ...
     * 
     * @param gs 
     * @return constexpr int 
     */
    constexpr int scenePriority(GameScene gs){
        switch (gs){
            case GameScene::GameOver: return 0;
            case GameScene::Clear:    return 1;
            case GameScene::Title:    return 2;
            case GameScene::Result:   return 3;
            case GameScene::Playing:  return 4;
        }
        return 999;  // #include <utility> std::unreachable();
    }

    /**
     * @brief 与えられたゲームシーンを比較して優先順位が高いものを返す
     * 
     * @param a 
     * @param b 
     * @return GameScene 
     */
    GameScene pickHigherPriority(GameScene a, GameScene b){
        return (scenePriority(a) <= scenePriority(b)) ? a : b;
    }
}

/**
 * @brief ゲームを始める関数
 * 
 * スコアリセットなど今のresetGameに相当
 * ここでscore, 残機, その他ゲーム全体の状態をリセット
 */
void SceneControl::startNewGame() {
    currentStageIndex = 0;
    resetGame();
}

/**
 * @brief 次のステージへ進む関数
 * 
 * @return true：次のステージへ
 * @return false：全ステージクリア 
 */
bool SceneControl::goToNextStage() {
    ++currentStageIndex;
    if (currentStageIndex >= static_cast<int>(StageConfig::STAGES.size())) {
        // 全ステージクリア
        return false;
    }
    return true;
}

/**
 * @brief ステージを読み込む関数
 * 
 * PlayingSceceへ入るたびに呼ばれる
 * 
 * @param stageIndex 
 * @param ctx 
 */
void SceneControl::loadStage(int stageIndex, GameContext& ctx){
    // stageIndexは負にならず，StageConfigのsize()を超えない
    assert(0 <= stageIndex && stageIndex < static_cast<int>(StageConfig::STAGES.size()));
    // ステージ定義を取得
    const auto& def = StageConfig::STAGES[stageIndex];
    // vector管理系のオブジェクトのクリア
    ctx.entityCtx.blocks.clear();
    ctx.entityCtx.enemies.clear();
    ctx.entityCtx.fireballs.clear();
    ctx.entityCtx.enemyBullets.clear();
    Texture& enemyTex = ctx.entityCtx.enemyTexture;
    // 敵のサイズ分メモリを予約
    ctx.entityCtx.enemies.reserve(def.enemySpawns.size());
    for(const auto& es : def.enemySpawns){
        // タイプごとに敵を生成
        switch(es.type){
        case EnemyType::Walker:
            ctx.entityCtx.enemies.emplace_back(std::make_unique<WalkerEnemy>(enemyTex));
            break;
        case EnemyType::Chaser:
            ctx.entityCtx.enemies.emplace_back(std::make_unique<ChaserEnemy>(enemyTex));
            break;
        case EnemyType::Jumper:
            ctx.entityCtx.enemies.emplace_back(std::make_unique<JumperEnemy>(enemyTex));
            break;
        case EnemyType::Turret:
            ctx.entityCtx.enemies.emplace_back(std::make_unique<TurretEnemy>(enemyTex));
            break;
        /*
        case EnemyType::Turret:
        ctx.entityCtx.enemies.emplace_back(std::make_unique<TurretEnemy>(enemyTex));
        break;
        */
        default:
            // ここには来ない想定なのでassertする
            assert(false && "Unknown EnemyType");
            break;
        }
        Enemy* e = ctx.entityCtx.enemies.back().get();
        e->applyEnemyParamForSpawn(es.x, es.y, es.speed);
    }
    
    // レベルファイル読み込み
    try{
        ctx.entityCtx.blocks = BlockLevelLoader::loadFromFile(def.levelFile);
    }catch(const std::exception& e){
        // 最低限のフォールバック
        // ハードコードで床を描画
        ctx.entityCtx.blocks.clear();
        ctx.entityCtx.blocks.push_back(Block{
            0.0,
            GameConfig::WINDOW_HEIGHT - 50.0,
            static_cast<double>(GameConfig::WINDOW_WIDTH),
            50.0, 
            BlockType::Standable
        });
        std::cerr << "[BlockLevelLoaderFallBack] " << e.what() << std::endl;
    }
    // ステージに配置されたブロックのRectをキャッシュ
    GameUtil::rebuildBlockRects(ctx.entityCtx.blocks, ctx.entityCtx.blockRectCaches);
    // worldInfoの再計算
    ctx.worldInfo.WorldWidth  = static_cast<double>(GameConfig::WINDOW_WIDTH); 
    ctx.worldInfo.WorldHeight = static_cast<double>(GameConfig::WINDOW_HEIGHT);
    for(const auto& b : ctx.entityCtx.blocks){
        ctx.worldInfo.WorldWidth  = std::max(ctx.worldInfo.WorldWidth, b.x + b.w);
        ctx.worldInfo.WorldHeight = std::max(ctx.worldInfo.WorldHeight, b.y + b.h);
    }
    // プレイヤーの位置初期化
    ctx.entityCtx.player.reset();
    ctx.entityCtx.player.setPosition(def.playerStart_X, def.playerStart_Y);
}

/**
 * @brief 各Sceneで呼ばれた要求を分解してシーンの遷移を実施する
 * 
 * @param geb 
 */
void SceneControl::consumeEvents(const GameEventBuffer& geb){
    // シーン遷移要求があったか
    bool hasSceneReq = false;
    // 決定される次のシーン
    GameScene nextScene = GameScene::Playing;   // Playingがデフォルト値

    // スコア計算用
    int scoreDelta = 0;

    // 遷移要求バッファを分解して遷移
    for(const auto& ev : geb.items()){
        // RequestSceneEventの値を指定して取り出す→中身はGameScene型のデータ
        if(const auto* rse = std::get_if<RequestSceneEvent>(&ev)){  // 引数はポインタ
            if(!hasSceneReq){   // Sceneの遷移要求が無い
                // 優先順位：GameOver > Clear > Title
                nextScene = rse->scene; // 取り出したGameSceneを次のシーンへ
                hasSceneReq = true;
            } else {    // すでにSceneの遷移要求があるとき
                // 優先順位に基づいて遷移するシーンを決定
                nextScene = pickHigherPriority(nextScene, rse->scene);
            }
            continue;
        }
        // スコアを加算するイベントがあるなら追加する
        if(const auto* ase = std::get_if<AddSceneEvent>(&ev)){
            scoreDelta += ase->delta;
            continue;
        }
    }
    // スコア加算イベントの結果を反映
    if(scoreDelta != 0){
        setScore(getScore() + scoreDelta);
    }
    // 最終的に決定されたシーン遷移要求を反映
    if(hasSceneReq){
        requestScene(nextScene);
    }
}
