#ifndef GAMEOVERSCENE_H
#define GAMEOVERSCENE_H

#include <memory>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "SimpleSceneBackground.hpp"

class Game;
class Renderer;
class TextTexture;

/**
 * @brief ゲームオーバーシーン管理
 * 
 */
class GameOverScene : public Scene{
private:
    // 演出用変数
    double blinkTimer = 0.0;
    bool blinkVisible= true;
    int remainingLives = 0;
    // 背景描画処理
    SimpleSceneBackground background;
    // 各種テキスト
    std::unique_ptr<TextTexture> gameOverText;
    std::unique_ptr<TextTexture> youAreDeadText;
    std::unique_ptr<TextTexture> returnTitleText;
    std::unique_ptr<TextTexture> returnStageText;
    std::unique_ptr<TextTexture> remainingLivesText;
    
public:
    // コンストラクタ
    GameOverScene(SceneControl& sc, GameContext& gc);
    // デストラクタ(継承しているので仮想関数)
    virtual ~GameOverScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

private:
    void updateGameOver(double delta);
};

#endif  // GAMEOVERSCENE_H
