#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"
#include "EnemySensor.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

struct DrawBounds;
class Game;
class Renderer;
class TextTexture;
struct InputState;

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
    // update内部で呼ばれる処理の分割
    void handlePlayingInput(const InputState& is);
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

    void gatherEnemySensors(std::vector<EnemySensor>& outEnemySensors);
    void runEnemyAI(double delta, const std::vector<EnemySensor>& sensors);

};

#endif  // PLAYINGSCENE_H