#ifndef WALKERENEMY_H
#define WALKERENEMY_H

#include "EnemySensor.hpp"
#include "Enemy.hpp"

class WalkerEnemy : public Enemy{
private:

public:
    WalkerEnemy(Texture& tex);
    ~WalkerEnemy() = default;
    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;
};

#endif  // WALKERENEMY_H