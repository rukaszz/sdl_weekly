#ifndef ENEMYAISYSTEM_H
#define ENEMYAISYSTEM_H

#include <vector>
#include <memory>

#include "EnemySensor.hpp"

class Player;
class Enemy;
struct Block;
struct WorldInfo;

class EnemyAISystem{
private:
    std::vector<std::unique_ptr<Enemy>>& enemies;
    const std::vector<Block>& blocks;
    const WorldInfo& world;
    const Player& player;
    std::vector<EnemySensor> sensors;
public:
    EnemyAISystem(
        std::vector<std::unique_ptr<Enemy>>& enemies_, 
        const std::vector<Block>& blocks_, 
        const WorldInfo& world_, 
        const Player& player_
    );
    ~EnemyAISystem() = default;
    // コピー禁止
    EnemyAISystem(const EnemyAISystem&) = delete;
    EnemyAISystem& operator=(const EnemyAISystem&) = delete;

    void update(double delta);
    void runEnemyAI(double delta);
private:
    // updateで呼ばれるセンサーの収集系関数群
    void gatherEnemySensors();
    EnemySensor buildEnemySensor(const Enemy& enemy, const EnemySensorContext& esc) const;
    void fillPlayerRelation(const Enemy& enemy, const EnemySensorContext& esc, EnemySensor& outSensor) const;
    void fillGroundAhead(const Enemy& enemy, const EnemySensorContext& esc, EnemySensor& outSensor) const;
    void fillWallAhead(const Enemy& enemy, const EnemySensorContext& esc, EnemySensor& outSensor) const;
};

#endif  // ENEMYAISYSTEM_H
