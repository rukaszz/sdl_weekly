#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

class Game;
class Renderer;

class TitleScene : public Scene{
protected:
    // ゲームのシーン(状態)
    Game& game;

public:
    TitleScene(Game& g);
    virtual ~TitleScene() = default;

    void handleEvent(const SDL_Event& e) override;
    void update(double delta) override;
    void render(Renderer& r) override;

    void onEnter(){};
    void onExit(){};
};

#endif  // TITLESCENE_H