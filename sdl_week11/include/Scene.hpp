#ifndef SCENE_H
#define SCENE_H

#include "SceneControl.hpp"

#include <SDL2/SDL.h>

struct GameContext;
class Game;
class Renderer;

enum class GameScene : size_t;

class Scene{
protected:
    // 抽象インターフェース
    SceneControl& ctrl;
    // Gameオブジェクトの参照
    GameContext& ctx;

public:
    Scene(SceneControl& sc, GameContext& gc);
    virtual ~Scene() = default;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void update(double delta) = 0;
    virtual void render() = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};

#endif  // SCENE_H