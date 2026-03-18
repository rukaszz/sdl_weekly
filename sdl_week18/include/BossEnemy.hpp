#ifndef BOSSENEMY_H
#define BOSSENEMY_H

#include "EnemySensor.hpp"
#include "Enemy.hpp"

class BossEnemy : public Enemy{
private:
    // TODO：後でマジックナンバーは消す
    int hp = 30;
    // ダメージを受けられるか
    bool canTakeDamage = true;
    // ボス弾の発射クールダウン
    double bossFireCooldownTimer = 0.0;
    // 発射要求
    bool fireRequested = false;
    // 攻撃方向
    Direction fireDir = Direction::Right;   // デフォルト右

public:
    BossEnemy(Texture& tex);
    ~BossEnemy() = default;
    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;

    // ダメージを受ける関数
    void takeDamage(int damage);
    // 発射要求
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

#endif  // BOSSENEMY_H
