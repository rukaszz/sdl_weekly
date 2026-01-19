#ifndef SCENECONTROL_H
#define SCENECONTROL_H

#include <SDL2/SDL.h>

struct GameContext;
enum class GameScene;

/**
 * @brief 抽象インターフェース
 * Gameクラスのうち，特定の呼びたい機能のみをSceneControlで呼ぶ
 * 依存性逆転の原則(dependency inversion principle)に従って，Gameへの依存を減らす
 * 
 */
class SceneControl{
private:
    // ステージ遷移用index
    int currentStageIndex = 0;

public:
    virtual void changeScene(GameScene id) = 0;
    virtual void resetGame() = 0;
    virtual uint32_t getScore() = 0;
    virtual void setScore(uint32_t v) = 0;
    virtual ~SceneControl() = default;

    void startNewGame();
    bool goToNextStage();
    void loadStage(int stageIndex, GameContext& ctx);

    // getter
    int getCurrentStageIndex() const{
        return currentStageIndex;
    }
};

#endif  // SCENECONTROL_H