#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include <memory>
#include <vector>

#include "Direction.hpp"

class Texture;
class Player;
class Enemy;
class BossEnemy;
class FireBall;
class EnemyBullet;
class SceneControl;
class IGameEvents;

struct Block;
struct WorldInfo;

/**
 * @brief 弾に関する処理をSceneから分離させる
 * 
 */
class ProjectileSystem{
private:
    // ファイアボール
    std::vector<std::unique_ptr<FireBall>>& fireballs;
    Texture& fireballTexture;
    // 敵弾
    std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets;
    Texture& enemyBulletTexture;

    const std::vector<Block>& blocks;
    const WorldInfo& world;
    // プレイヤー弾の発射間隔
    double playerFireCooldownTimer = 0.0;

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
    // プレイヤーはファイアボール撃てるか
    bool canSpawnPlayerFireball() const;
    // ファイアボールが撃てるか判定してから撃つ関数
    bool trySpawnPlayerFireball(double x, double y, Direction dir);

    // 弾発射用関数
    void spawnEnemyBullet(double x, double y, Direction dir);
    // 敵AIの判断に結果から敵弾生成(EnemyBullet)
    void spawnEnemyBulletsFromEnemies(std::vector<std::unique_ptr<Enemy>>& enemies);
    // Bossの弾発射処理
    void spawnBossBullets(BossEnemy& boss);

    // その他関数
    // ステージロード時のキャッシュ更新用
    void onStageLoaded();
    // 更新系はplayer/enemy両方更新する
    void update(double delta);
    // 当たり判定(弾 vs entity)
    // bossはボスステージでのみ使用するのでnullptr判定のためにポインタ渡し
    void resolveCollisions(
        Player& player, 
        std::vector<std::unique_ptr<Enemy>>& enemies, 
        BossEnemy& boss,
        bool bossBattleActive,  
        IGameEvents& events
    );
    // 生存管理(非活性・画面外は消す)
    void cleanup();

private:
    // fireball vs enemy
    void resolveFireballEnemyCollision(std::vector<std::unique_ptr<Enemy>>& enemies, IGameEvents& events);
    // player vs enemy bullet
    void resolvePlayerEnemyBulletCollision(Player& player, IGameEvents& events);
    // fireball vs boss
    void resolveFireballBossCollision(BossEnemy& boss, IGameEvents& events);
    // 活性状態のファイアボール計測
    int countActivePlayerFireballs() const;
};

#endif  // PROJECTILESYSTEM_H
