#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

struct GameContext;
class Game;
class Renderer;

enum class GameScene;

/**
 * @brief 抽象インターフェース
 * Gameクラスのうち，特定の呼びたい機能のみをSceneControlで呼ぶ
 * 依存性逆転の原則(dependency inversion principle)に従って，Gameへの依存を減らす
 * 
 */
class SceneControl{
public:
    virtual void changeScene(GameScene id) = 0;
    virtual void resetGame() = 0;
    virtual uint32_t getScore() = 0;
    virtual void setScore(uint32_t v) = 0;
    virtual ~SceneControl() = default;
};

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