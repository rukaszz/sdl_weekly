#ifndef PLAYINGSCENE_H
#define PLAYINGSCENE_H

#include "Scene.hpp"

#include <SDL2/SDL.h>

struct DrawBounds;
class Game;
class Renderer;

// enum class PlayerEnemyCollisionResult{
//     None, 
//     StompEnemy, 
//     PlayerHit, 
//     Count, 
// };

class PlayingScene : public Scene{
private:

public:
    PlayingScene(SceneControl& sc, GameContext& gc);
    virtual ~PlayingScene() = default;

    virtual void handleEvent(const SDL_Event& e) override;
    virtual void update(double delta) override;
    virtual void render() override;
    virtual void onEnter() override;
    virtual void onExit() override;

    void updateScore(double delta);
    void updateEntities(double delta, DrawBounds b);
    void updateCamera();
    void checkCollision();
    void detectCollision();
    void resolveBlockCollision();
    void resolveEnemyCollision();
    void hasFallenToGameOver();
    // static PlayerEnemyCollisionResult resolvePlayerEnemyCollision(
    //     const SDL_Rect& playerRect, 
    //     double playerPrevFeet, 
    //     double playerNewFeet, 
    //     double playerVv,
    //     const SDL_Rect& enemyRect 
    // );
};

#endif  // PLAYINGSCENE_H