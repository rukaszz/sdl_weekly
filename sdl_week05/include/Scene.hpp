#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

struct GameContext;
class Game;
class Renderer;

class Scene{
protected:
    Game& game;
    // Gameオブジェクトの参照
    const GameContext& ctx;

public:
    Scene(Game& g, const GameContext& context);
    virtual ~Scene() = default;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void update(double delta) = 0;
    virtual void render() = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};

#endif  // SCENE_H