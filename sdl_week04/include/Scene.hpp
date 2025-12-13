#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

class Game;
class Renderer;

class Scene{
protected:
    // Gameオブジェクトの参照
    Game& game;

public:
    Scene(Game& g);
    virtual ~Scene() = default;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void update(double delta) = 0;
    virtual void render(Renderer& r) = 0;

    virtual void onEnter(){};
    virtual void onExit(){};
};

#endif  // SCENE_H