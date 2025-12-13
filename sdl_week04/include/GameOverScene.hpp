#ifndef GAMEOVERSCENE_H
#define GAMEOVERSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

class Game;
class Renderer;

class GameOverScene : public Scene{
protected:
    // ゲームのシーン(状態)
    Game& game;

public:
    GameOverScene(Game& g);
    virtual ~GameOverScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render(Renderer& r) override;

    virtual void onEnter(){};
    virtual void onExit(){};
};

#endif  // GAMEOVERSCENE_H