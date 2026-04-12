#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <vector>
#include <memory>

#include <SDL2/SDL.h>

class IGameEvents;

class SceneControl;
class Player;
class Enemy;
struct Block;
struct WorldInfo;

class CollisionSystem{
private:
    Player& player;
    std::vector<std::unique_ptr<Enemy>>& enemies;
    const std::vector<Block>& blocks;
    const std::vector<SDL_Rect>& blockRects;
    const WorldInfo& world;
public:
    CollisionSystem(
        Player& player_,
        std::vector<std::unique_ptr<Enemy>>& enemies_, 
        const std::vector<Block>& blocks_, 
        const std::vector<SDL_Rect>& blockRects_,
        const WorldInfo& world_
    );
    ~CollisionSystem() = default;
    // ステージロード時のキャッシュ更新用
    void onStageLoaded();
    // updadeで呼ばれるcollision処理の関数群
    // provateでまとめてresolve()関数で読んでいたが，判定の順序整理の過程で廃止している
    void resolveSpecialBlockCollision(IGameEvents& events);
    void resolveEnemyCollision(IGameEvents& events);
    void resolveBlockHits(IGameEvents& events);
    // void checkFallDeath(SceneControl& ctrl);
    void checkFallDeath(IGameEvents& events);

};

#endif  // COLLISIONSYSTEM_H
