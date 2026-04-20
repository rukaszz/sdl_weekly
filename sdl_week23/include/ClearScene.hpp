#ifndef CLEARSCENE_H
#define CLEARSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <memory>

class Game;
class Renderer;
class TextTexture;

class ClearScene : public Scene{
private:
    std::unique_ptr<TextTexture> clearText;
    std::unique_ptr<TextTexture> returnTitleText;
    std::unique_ptr<TextTexture> NextStageText;
    double blinkTimer = 0.0;
    bool blinkVisible= true;
public:
    ClearScene(SceneControl& sc, GameContext& gc);
    virtual ~ClearScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

    void updateClear(double delta);
};

#endif  // CLEARSCENE_H