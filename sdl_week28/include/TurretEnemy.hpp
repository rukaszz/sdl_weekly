#ifndef TURRETENEMY_H
#define TURRETENEMY_H

#include "TurretConfig.hpp"
#include "EnemySensor.hpp"
#include "Direction.hpp"
#include "Enemy.hpp"

class Texture;

class TurretEnemy : public Enemy{
private:
    double fireTimer = TurretConfig::FIRE_INTERVAL;     // 初弾もちょっと待つ
    bool fireRequested = false;
    Direction fireDir = Direction::Right;   // デフォルト右
public:
    TurretEnemy(Texture& tex);
    void think(double delta, const EnemySensor& es) override;
    bool consumeFireRequest(Direction& outDir) noexcept;
    // 弾を発射できるかを返す
    bool shouldFire() const noexcept{
        return fireRequested;
    }
    // 弾の射出方向を返す
    Direction getFireDirection() const noexcept{
        return fireDir;
    }
};

#endif  // TURRETENEMY_H
