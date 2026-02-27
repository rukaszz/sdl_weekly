#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <vector>
#include <memory>

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
    const WorldInfo& world;
public:
    CollisionSystem(
        Player& player_,
        std::vector<std::unique_ptr<Enemy>>& enemies_, 
        const std::vector<Block>& blocks_, 
        const WorldInfo& world_
    );
    ~CollisionSystem() = default;
    void resolve(SceneControl& ctrl);
    void checkFallDeath(SceneControl& ctrl);
private:
    // updadeで呼ばれるcollision処理の関数群
    void resolveBlockCollision(SceneControl& ctrl);
    void resolveEnemyCollision(SceneControl& ctrl);
};

#endif  // COLLISIONSYSTEM_H
