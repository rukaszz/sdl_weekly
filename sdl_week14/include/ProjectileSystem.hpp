#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include "Texture.hpp"
#include "FireBall.hpp"
#include "EnemyBullet.hpp"
#include "GameContext.hpp"

#include <memory>
#include <vector>

class ProjectileSystem{
public: 
    ProjectileSystem(std::vector<std::unique_ptr<FireBall>>& playerBullets, 
                     std::vector<std::unique_ptr<EnemyBullet>>& enemyBellets, 
                     const std::vector<Block>& blocks, 
                     const WorldInfo& world, 
                     Texture& fireballTexture, 
                     Texture& enemyBulletTexture
    );
    // 弾発射系
    void spawnPlayerFireball(double x, double y, Direction dir);
    void spawnTurretBullets(std::vector<std::unique_ptr<Enemy>>& enemies);

    // その他関数
    // 更新系はplayer/enemy両方更新する
    void update(double delta);
    void collide(Player& player, std::vector<std::unique_ptr<Enemy>>& enemies, SceneControl& ctrl);
    void cleanup();
};

#endif  // PROJECTILESYSTEM_H
