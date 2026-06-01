#ifndef TITLESCENE_H
#define TITLESCENE_H

#include <memory>

#include <SDL2/SDL.h>

#include "Scene.hpp"
#include "SimpleSceneBackground.hpp"

class Game;
class Renderer;
class TextTexture;

/**
 * @brief タイトルシーンを管理するクラス
 * 
 */
class TitleScene : public Scene{
private:
    // タイトル処理用変数
    double titleFade = 0.0; // 0.0 .. 1.0
    double blinkTimer = 0.0;
    bool blinkVisible= true;

    // 背景描画処理用
    SimpleSceneBackground background;
    // 各種テキスト
    std::unique_ptr<TextTexture> gameTitleText;
    std::unique_ptr<TextTexture> gameEnterText;
    std::unique_ptr<TextTexture> gameLeaveText;
    std::unique_ptr<TextTexture> howToPlayText;

public:
    // コンストラクタ
    TitleScene(SceneControl& sc, GameContext& gc);
    // デストラクタ(継承しているので仮想関数)
    virtual ~TitleScene() = default;

    void handleEvent(const SDL_Event& e) override;
    void update(double delta) override;
    void render() override;
    void onEnter() override;
    void onExit() override;

private:
    void updateTitle(double delta);
};

#endif  // TITLESCENE_H
