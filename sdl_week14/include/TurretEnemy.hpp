#ifndef TURRETENEMY_H
#define TURRETENEMY_H

#include "EnemySensor.hpp"
#include "Direction.hpp"
#include "Enemy.hpp"

class Texture;

class TurretEnemy : public Enemy{
private:
    double fireInterval = 1.5;  // 秒
    double fireTimer = 0.0;
    bool fireRequested = false;
    Direction fireDir = Direction::Right;   // デフォルト右
public:
    TurretEnemy(Texture& tex);
    void think(double delta, const EnemySensor& es) override;
    // 弾を発射できるかを返す
    bool shouldFire() const noexcept{
        return fireRequested;
    }
    // 弾発射の要求をリセット
    void clearFireRequest() noexcept{
        fireRequested = false;
    }
    // 弾の射出方向を返す
    Direction getFireDirection() const noexcept{
        return fireDir;
    }
};

#endif  // TURRETENEMY_H
