#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

struct DrawBounds;
class Game;
class Renderer;

class PlayingScene : public Scene{
private:

public:
    PlayingScene(Game& g, const GameContext& context);
    virtual ~PlayingScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;
    virtual void onEnter() override;
    virtual void onExit() override;

    void updateScore(double delta);
    void updateEntities(double delta, DrawBounds b);
    void checkCollision();
};

#endif  // PLAYINGSCENE_H