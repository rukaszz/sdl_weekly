#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <memory>

class Game;
class Renderer;
class TextTexture;

class TitleScene : public Scene{
private:
    std::unique_ptr<TextTexture> gameTitleText;
    std::unique_ptr<TextTexture> gameEnterText;
    // タイトル処理
    double titleFade = 0.0; // 0.0 .. 1.0
    double blinkTimer = 0.0;
    bool blinkVisible= true;

public:
    TitleScene(SceneControl& sc, GameContext& gc);
    virtual ~TitleScene() = default;

    void handleEvent(const SDL_Event& e) override;
    void update(double delta) override;
    void render() override;
    void onEnter() override;
    void onExit() override;

    void updateTitle(double delta);
};

#endif  // TITLESCENE_H