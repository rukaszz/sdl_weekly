#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

#include <memory>

struct DrawBounds;
class Game;
class Renderer;
class TextTexture;

class PlayingScene : public Scene{
private:
    std::unique_ptr<TextTexture> debugText;

public:
    PlayingScene(SceneControl& sc, GameContext& gc);
    virtual ~PlayingScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    void updateScore(double delta);
    void updateEntities(double delta, DrawBounds b);
    void updateCamera();
    void checkCollision();
    void detectCollision();
    void resolveBlockCollision();
    void resolveEnemyCollision(double prevFeet);
    void hasFallenToGameOver();
    bool checkBoundsforFireBalls(SDL_Rect fr, const double world_W, const double world_H);
    void cleanupFireBalls();
    void resolveFireBallEnemyCollision();

};

#endif  // PLAYINGSCENE_H