#ifndef CLEARSCENE_H
#define CLEARSCENE_H

#include <memory>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "SimpleSceneBackground.hpp"

class Game;
class Renderer;
class TextTexture;

/**
 * @brief ステージクリアシーンの管理
 * 
 */
class ClearScene : public Scene{
private:
    // 演出用変数
    double blinkTimer = 0.0;
    bool blinkVisible= true;
    // 背景
    SimpleSceneBackground background;
    // テキスト
    std::unique_ptr<TextTexture> clearText;
    std::unique_ptr<TextTexture> returnTitleText;
    std::unique_ptr<TextTexture> nextStageText;
    
public:
    // コンストラクタ
    ClearScene(SceneControl& sc, GameContext& gc);
    // デストラクタ(継承しているので仮想関数)
    virtual ~ClearScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

private:
    void updateClear(double delta);
};

#endif  // CLEARSCENE_H
