#ifndef BOSSENEMY_H
#define BOSSENEMY_H

#include "Enemy.hpp"
#include "EnemySensor.hpp"

class BossEnemy : public Enemy{
private:
    // TODO：後でマジックナンバーは消す
    int hp = 0;
    int maxHp = 0;  // HPバーの比率計算用
    // ダメージを受けた際の一時的な無敵時間
    double invincibleTimer = 0.0;
    // ボス弾の発射クールダウン
    double bossFireCooldownTimer = 0.0;
    // 発射要求
    bool fireRequested = false;
    // 攻撃方向
    Direction fireDir = Direction::Left;   // デフォルト左

public:
    explicit BossEnemy(Texture& tex);
    ~BossEnemy() = default;

    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;
    // 更新関数
    void update(double delta, const InputState& is, DrawBounds b, const std::vector<Block>& blocks) override;
    // 描画関数(無敵時間表現用)
    void draw(Renderer& renderer, const Camera& camera) override;
    // ステージロード時の初期化
    void reset(int initHp, double spawn_X, double spawn_Y);
    // ダメージを受ける関数
    void takeDamage(int damage);
    // 発射要求
    bool consumeFireRequest(Direction& outDir) noexcept;

    // getter/setter
    int getHp() const noexcept{
        return hp;
    }
    int getMaxHp() const noexcept{
        return maxHp;
    }
    // 無敵時間中かの判定を返す
    bool canTakeDamage() const noexcept{
        return (invincibleTimer <= 0.0 && isAlive());
    }
private:
    // 描画できるかの判定
    bool shouldRender() const;
};

#endif  // BOSSENEMY_H
