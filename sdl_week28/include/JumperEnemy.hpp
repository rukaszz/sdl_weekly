#ifndef JUMPERENEMY_H
#define JUMPERENEMY_H

#include "JumperConfig.hpp"
#include "EnemySensor.hpp"
#include "Enemy.hpp"

#include <random>

class Texture;

class JumperEnemy : public Enemy{
private:
    double jumpCooldown = 0.0;                              // ジャンプの休息時間
    
    // ランダム要素
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;    // 一様分布
    // 次のジャンプ間隔の決定
    double nextInterval();

public:
    JumperEnemy(Texture& tex);
    ~JumperEnemy() = default;
    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;
};

#endif  // JUMPERENEMY_H
