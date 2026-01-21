#include "SceneControl.hpp"

#include "GameConfig.hpp"
#include "StageConfig.hpp"
#include "GameContext.hpp"
#include "Block.hpp"
#include "BlockLevelLoader.hpp"
#include "Enemy.hpp"
// #include "Game.hpp"

#include <cassert>
#include <algorithm>
#include <memory>

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
    // ブロック/敵のクリア
    ctx.entityCtx.blocks.clear();
    ctx.entityCtx.enemies.clear();
    Texture& enemyTex = ctx.entityCtx.enemyTexture;
    // for(int i = 0;i < 5; ++i){
    //     enemies.push_back(std::make_unique<Enemy>(*enemyTex));
    // }
    ctx.entityCtx.enemies.reserve(def.enemySpawns.size());
    for(const auto& es : def.enemySpawns){
        /* c++17: 
        auto& enemyPtr = ctx.entityCtx.enemies.emplace_back(std::make_unique<Enemy>(enemyTex));
        enemyPtr->applyEnemyParamForSpawn(es.x, es.y, es.speed);
        */
        // c++ 11/14
        ctx.entityCtx.enemies.emplace_back(std::make_unique<Enemy>(enemyTex));
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
    }
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
    // 敵の位置初期化
    // for(auto& e : ctx.entityCtx.enemies){
    //     e->reset(ctx.randomCtx.random, ctx.randomCtx.distX, ctx.randomCtx.distY, ctx.randomCtx.distSpeed);
    // }
}
