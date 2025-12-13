#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

class Game;
class Renderer;

class PlayingScene : public Scene{
protected:
    // ゲームのシーン(状態)
    Game& game;

public:
    PlayingScene(Game& g);
    virtual ~PlayingScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render(Renderer& r) override;

    virtual void onEnter(){};
    virtual void onExit(){};
};

#endif  // PLAYINGSCENE_H