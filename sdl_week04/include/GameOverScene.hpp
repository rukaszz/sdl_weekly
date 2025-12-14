#ifndef GAMEOVERSCENE_H
#define GAMEOVERSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

class Game;
class Renderer;

class GameOverScene : public Scene{
private:
    double blinkTimer = 0.0;
    bool blinkVisible= true;
public:
    GameOverScene(Game& g);
    virtual ~GameOverScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;

    virtual void onEnter();
    virtual void onExit();

    void updateGameOver(double delta);
};

#endif  // GAMEOVERSCENE_H