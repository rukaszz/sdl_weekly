#ifndef SCENECONTROL_H
#define SCENECONTROL_H

#include <vector>
#include <cassert>

#include "GameScene.hpp"
#include "StageDefinition.hpp"
#include "SceneConfig.hpp"

#include <SDL2/SDL.h>

class GameEventBuffer;
struct GameContext;

/**
 * @brief GameクラスとSceneクラスの依存を緩め，ステージ進行を担うクラス
 * Gameクラスのうち，特定の呼びたい機能のみをSceneControlで呼ぶ
 * 依存性逆転の原則(dependency inversion principle)に従って，Gameへの依存を減らす
 * 
 */
class SceneControl{
private:
    // ステージ遷移用index
    int currentStageIndex = 0;
    // 各ステージの定義(Jsonの解析結果が入る)
    std::vector<StageDefinition> stageDefinitions;
    // 残機
    int lives = 3;

protected:
    // シーン変更処理のヘルパとしてしばらく残す
    virtual void changeScene(GameScene id) = 0;

public:
    // コンストラクタ
    SceneControl() = default;
    virtual void resetGame() = 0;
    virtual uint32_t getScore() = 0;
    virtual void setScore(uint32_t v) = 0;
    virtual ~SceneControl() = default;

    // ステージ定義取得
    void initStages(std::vector<StageDefinition> defs);
    // 新規ゲーム開始処理
    void startNewGame();
    // ステージ遷移(インデックスのインクリメント)
    bool goToNextStage();
    // ステージオブジェクトの読み込み
    void loadStage(int stageIndex, GameContext& ctx);
    // 各種イベントの処理
    void consumeEvents(const GameEventBuffer& geb);
    // 残機消費
    bool tryConsumeLife();

    virtual void requestScene(GameScene id) = 0;

    // getter
    const StageDefinition& getCurrentStageDefinition() const;
    int getCurrentStageIndex() const{
        return currentStageIndex;
    }
    // stageDefinitionsのサイズ=ステージ数
    int getStageCount() const{
        return static_cast<int>(stageDefinitions.size());
    }
    // 現在の残機
    int getLives() const{
        return lives;
    }
    bool currentStageHasBoss() const;

private:
    // loadStage()のヘルパ関数群
    void clearStageEntities(GameContext& ctx);
    void loadEnemies(const StageDefinition& def, GameContext& ctx);
    void loadBlocks(const StageDefinition& def, GameContext& ctx);
    void loadItems(const StageDefinition& def, GameContext& ctx);
    void rebuildWorldInfo(GameContext& ctx);
    void resetPlayer(const StageDefinition& def, GameContext& ctx);
    void loadBoss(const StageDefinition& def, GameContext& ctx);
};

#endif  // SCENECONTROL_H
