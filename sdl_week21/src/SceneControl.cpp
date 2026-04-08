#include "SceneControl.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>

#include "Block.hpp"
#include "BlockLevelLoader.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "BossEnemy.hpp"
#include "Item.hpp"
#include "WalkerEnemy.hpp"
#include "ChaserEnemy.hpp"
#include "JumperEnemy.hpp"
#include "TurretEnemy.hpp"
#include "FireBall.hpp"
#include "EnemyBullet.hpp"
#include "GameEventBuffer.hpp"
#include "StageDefinitionLoader.hpp"

#include "WorldInfo.hpp"
#include "GameScene.hpp"
#include "GameConfig.hpp"
#include "StageDefinition.hpp"
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
            case GameScene::Count: /* Do nothing */ break;
        }
        assert(false);
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
}   // namespace

/**
 * @brief 起動時に1回呼ばれ，ステージの定義をメンバ変数へmoveする 
 * 
 * @param defs 
 */
void SceneControl::initStages(std::vector<StageDefinition> defs){
    if(defs.empty()){
        throw std::runtime_error("No stage definitions loaded");
    }    
    stageDefinitions = std::move(defs);
}

/**
 * @brief ゲームを始める関数
 * 
 * スコアリセットなど今のresetGameに相当
 * ここでscore, 残機, その他ゲーム全体の状態をリセット
 */
void SceneControl::startNewGame() {
    currentStageIndex = 0;  // ステージインデックスを0
    lives = INITIAL_LIVES;  // 残機を3
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
    // if (currentStageIndex >= static_cast<int>(StageConfig::STAGES.size())) {
    if (currentStageIndex >= static_cast<int>(stageDefinitions.size())) {
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
    assert(0 <= stageIndex && stageIndex < static_cast<int>(stageDefinitions.size()));
    // ステージ定義を取得
    const auto& def = stageDefinitions[stageIndex];
    clearStageEntities(ctx);
    loadEnemies(def, ctx);
    loadBlocks(def, ctx);
    loadItems(def, ctx);
    rebuildWorldInfo(ctx);
    resetPlayer(def, ctx);
    loadBoss(def, ctx);
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
        if(const auto* ase = std::get_if<AddScoreEvent>(&ev)){
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

/**
 * @brief 残機を消費してコンテニューできるかを調べる
 * 
 * @return true：コンテニュー可能 
 * @return false：コンテニュー不可 
 */
bool SceneControl::tryConsumeLife(){
    --lives;
    return lives > 0;
}

/**
 * @brief Sceneオブジェクトでステージ定義を取得する用関数
 * 
 * @return const StageDefinition& 
 */
const StageDefinition& SceneControl::getCurrentStageDefinition() const{
    // ステージインデックスがステージ定義の数より大きいのはNG
    assert(0 <= currentStageIndex
        && currentStageIndex < static_cast<int>(stageDefinitions.size()));
    return stageDefinitions[currentStageIndex];
}

/**
 * @brief 現在のステージがボス戦を持つか判定
 * 
 * @return true 
 * @return false 
 */
bool SceneControl::currentStageHasBoss() const{
    // ステージインデックスがステージ定義の数より大きいのはNG
    assert(
        0 <= currentStageIndex
          && currentStageIndex < static_cast<int>(stageDefinitions.size())
    );
    return stageDefinitions[currentStageIndex].bossBattleDef.enabled;
}

// loadStage()ヘルパ
/**
 * @brief 各種のvector管理のオブジェクトの初期化
 * 
 * @param ctx 
 */
void SceneControl::clearStageEntities(GameContext& ctx){
    ctx.entityCtx.blocks.clear();
    ctx.entityCtx.blockRectCaches.clear();
    ctx.entityCtx.enemies.clear();
    ctx.entityCtx.fireballs.clear();
    ctx.entityCtx.enemyBullets.clear();
    ctx.entityCtx.items.clear();
}

/**
 * @brief Enemy系のオブジェクト読み込み処理
 * 
 * @param def 
 * @param ctx 
 */
void SceneControl::loadEnemies(const StageDefinition& def, GameContext& ctx){
    // テクスチャの読み込み
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
        default:
            // ここには来ない想定なのでassertする
            assert(false && "Unknown EnemyType");
            break;
        }
        // 敵のスポーン
        ctx.entityCtx.enemies.back()->applyEnemyParamForSpawn(es.x, es.y, es.speed);
    }
}

/**
 * @brief レベルファイルに基づいたブロックの配置関数
 * 
 * @param def 
 * @param ctx 
 */
void SceneControl::loadBlocks(const StageDefinition& def, GameContext& ctx){
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
}

/**
 * @brief ステージ定義に基づいてアイテムを配置する関数
 * 
 * @param def 
 * @param ctx 
 */
void SceneControl::loadItems(const StageDefinition& def, GameContext& ctx){
    // アイテムの配置
    ctx.entityCtx.items.reserve(def.itemSpawns.size());
    for(const auto& is : def.itemSpawns){
        ctx.entityCtx.items.emplace_back(is.x, is.y, is.type);
    }
}

/**
 * @brief ブロックの読み込みに基づいてworldInfoの拡張を実施する関数
 * 
 * @param ctx 
 */
void SceneControl::rebuildWorldInfo(GameContext& ctx){
    // デフォルト値を設定
    ctx.worldInfo.WorldWidth  = static_cast<double>(GameConfig::WINDOW_WIDTH);
    ctx.worldInfo.WorldHeight = static_cast<double>(GameConfig::WINDOW_HEIGHT);
    // worldInfoの再計算
    for(const auto& b : ctx.entityCtx.blocks){
        ctx.worldInfo.WorldWidth  = std::max(ctx.worldInfo.WorldWidth, b.x + b.w);
        ctx.worldInfo.WorldHeight = std::max(ctx.worldInfo.WorldHeight, b.y + b.h);
    }
}

/**
 * @brief プレイヤーの初期化
 * 
 * @param def 
 * @param ctx 
 */
void SceneControl::resetPlayer(const StageDefinition& def, GameContext& ctx){
    ctx.entityCtx.player.resetForStageTransition();
    ctx.entityCtx.player.setPosition(def.playerStart_X, def.playerStart_Y);
}

/**
 * @brief ボスの初期化
 * 
 * @param def 
 * @param ctx 
 */
void SceneControl::loadBoss(const StageDefinition& def, GameContext& ctx){
    // ステージ定義にボスの情報があれば読み込む
    if(!def.bossBattleDef.enabled){
        return;
    }
    // 初期化
    ctx.entityCtx.boss.reset(
        def.bossBattleDef.hp,
        def.bossBattleDef.bossSpawn_X,
        def.bossBattleDef.bossSpawn_Y
    );
}
