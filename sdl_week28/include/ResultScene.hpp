#ifndef RESULTSCENE_H
#define RESULTSCENE_H

#include <memory>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "SimpleSceneBackground.hpp"

class Game;
class Renderer;
class TextTexture;

/**
 * @brief リザルト画面管理用
 * 
 */
class ResultScene : public Scene{
private:
    // 文字列処理用変数
    double blinkTimer = 0.0;
    bool blinkVisible= true;
    // 背景描画処理用
    SimpleSceneBackground background;
    // 各種テキスト
    std::unique_ptr<TextTexture> resultText;
    std::unique_ptr<TextTexture> returnTitleText;
    std::unique_ptr<TextTexture> praiseText;
    std::unique_ptr<TextTexture> stageText;

public:
    // コンストラクタ
    ResultScene(SceneControl& sc, GameContext& gc);
    // デストラクタ(継承しているので仮想関数)
    virtual ~ResultScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

private:
    void updateBlink(double delta);
};

#endif  // RESULTSCENE_H
