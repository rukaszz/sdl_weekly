#ifndef GAMEOVERSCENE_H
#define GAMEOVERSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <memory>

class Game;
class Renderer;
class TextTexture;

class GameOverScene : public Scene{
private:
    std::unique_ptr<TextTexture> gameOverText;
    std::unique_ptr<TextTexture> returnTitleText;
    double blinkTimer = 0.0;
    bool blinkVisible= true;
public:
    GameOverScene(SceneControl& sc, GameContext& gc);
    virtual ~GameOverScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter() override;
    virtual void onExit() override;

    void updateGameOver(double delta);
};

#endif  // GAMEOVERSCENE_H