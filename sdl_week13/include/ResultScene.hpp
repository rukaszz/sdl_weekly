#ifndef RESULTSCENE_H
#define RESULTSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <memory>

class Game;
class Renderer;
class TextTexture;

class ResultScene : public Scene{
private:
    std::unique_ptr<TextTexture> resultText;
    std::unique_ptr<TextTexture> returnTitleText;
    std::unique_ptr<TextTexture> praiseText;
    std::unique_ptr<TextTexture> stageText;
    double blinkTimer = 0.0;
    bool blinkVisible= true;
public:
    ResultScene(SceneControl& sc, GameContext& gc);
    virtual ~ResultScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

    void updateBlink(double delta);
};

#endif  // RESULTSCENE_H