#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>

#include "Particle.hpp"

class Renderer;
struct Camera;

/**
 * @brief パーティクル用サブシステム
 * イベント駆動型
 * 
 */
class ParticleSystem{
private:
    std::vector<Particle> particles;

public:
    // コイン取得時のパーティクル
    void spawnCoinSpark(double x, double y);
    // 敵撃破時のパーティクル
    void spawnEnemyBurst(double x, double y);
    // プレイヤー死亡時のパーティクル
    void spawnPlayerDeath(double x, double y);
    // ブロック破壊時のパーティクル
    void spawnBlockDebris(double x, double y);
    // 更新関数
    void update(double delta);
    // 描画
    void render(Renderer& renderer, const Camera& camera) const;
    // クリア
    void clear();

private:
    // パーティクル出現上限のチェック
    bool canSpawn(std::size_t count) const;
};

#endif  // PARTICLESYSTEM_H
