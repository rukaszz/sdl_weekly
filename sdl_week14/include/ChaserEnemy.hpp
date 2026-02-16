#ifndef CHASERENEMY_H
#define CHASERENEMY_H

#include "EnemySensor.hpp"
#include "Enemy.hpp"

class ChaserEnemy : public Enemy{
private:

public:
    ChaserEnemy(Texture& tex);
    ~ChaserEnemy() = default;
    // 敵の行動判断用
    void think(double delta, const EnemySensor& es) override;
};

#endif  // CHASERENEMY_H