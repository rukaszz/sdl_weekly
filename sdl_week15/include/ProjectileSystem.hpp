#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include <memory>
#include <vector>

#include "Direction.hpp"

class Texture;
class Player;
class Enemy;
class FireBall;
class EnemyBullet;
class SceneControl;

struct Block;
struct WorldInfo;

/**
 * @brief 弾に関する処理をSceneから分離させる
 * 
 */
class ProjectileSystem{
public: 
    ProjectileSystem(
        std::vector<std::unique_ptr<FireBall>>& fireballs,
        std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets,
        const std::vector<Block>& blocks,
        const WorldInfo& worldInfo,
        Texture& fireballTexture,
        Texture& enemyBulletTexture
    );
    // 弾発射系
    // プレイヤーの入力でプレイヤー弾(ファイアボール)生成
    void spawnPlayerFireball(double x, double y, Direction dir);
    // 敵AIの判断に結果から敵弾生成(EnemyBullet)
    void spawnEnemyBulletsFromEnemies(std::vector<std::unique_ptr<Enemy>>& enemies);

    // その他関数
    // ステージロード時のキャッシュ更新用
    void onStageLoaded();
    // 更新系はplayer/enemy両方更新する
    void update(double delta);
    // 当たり判定(弾 vs entity)
    void resolveCollisions(Player& player, std::vector<std::unique_ptr<Enemy>>& enemies, SceneControl& ctrl);
    // 生存管理(非活性・画面外は消す)
    void cleanup();
private:
    std::vector<std::unique_ptr<FireBall>>& fireballs;
    std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets;
    const std::vector<Block>& blocks;
    const WorldInfo& world;
    Texture& fireballTexture;
    Texture& enemyBulletTexture;

    void resolveFireballEnemyCollision(std::vector<std::unique_ptr<Enemy>>& enemies, SceneControl& ctrl);
    void resolvePlayerEnemyBulletCollision(Player& player, SceneControl& ctrl);

};

#endif  // PROJECTILESYSTEM_H
