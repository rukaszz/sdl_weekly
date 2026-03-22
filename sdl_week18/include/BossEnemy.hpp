#ifndef BOSSENEMY_H
#define BOSSENEMY_H

#include "Character.hpp"
#include "EnemySensor.hpp"
#include "Enemy.hpp"

struct StageDefinition;

class BossEnemy : public Enemy{
private:
    // TODO：後でマジックナンバーは消す
    int hp = 30;
    // ダメージを受けた際の一時的な無敵時間
    double damageInvincibleTimer = 0.0;
    // ボス弾の発射クールダウン
    double bossFireCooldownTimer = 0.0;
    // 発射要求
    bool fireRequested = false;
    // 攻撃方向
    Direction fireDir = Direction::Right;   // デフォルト右
    // 負けたか(倒されたらtrue)
    bool defeated = false;

public:
    explicit BossEnemy(Texture& tex);
    ~BossEnemy() = default;

    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;
    // 更新関数
    void update(double delta, const InputState& is, DrawBounds b, const std::vector<Block>& blocks) override;
    // ステージロード時の初期化
    void reset(int initHp, double spawn_X, double spawn_Y);
    // ダメージを受ける関数
    void takeDamage(int damage);
    // 発射要求
    bool consumeFireRequest(Direction& outDir) noexcept;

    // getter/setter
    // ボスの敗北状況を返す
    bool isDefeated() const noexcept{
        return defeated;
    }
    // 弾を発射できるかを返す
    bool shouldFire() const noexcept{
        return fireRequested;
    }
    // 弾の射出方向を返す
    Direction getFireDirection() const noexcept{
        return fireDir;
    }
    // HPの設定
    void setInitialHp(int v) noexcept{
        hp = v;
    }
};

#endif  // BOSSENEMY_H
