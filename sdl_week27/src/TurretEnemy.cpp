#include "TurretEnemy.hpp"

#include "TurretConfig.hpp"
#include "EnemySensor.hpp"
#include "Direction.hpp"

#include <cmath>

/**
 * @brief Construct a new Turret Enemy:: Turret Enemy object
 * 
 * @param tex 
 */
TurretEnemy::TurretEnemy(Texture& tex)
    : Enemy(tex)
{

}

/**
 * @brief TurretEnemyの判断用関数
 * 弾を撃つなどの判断をする
 * 
 * @param delta 
 * @param es 
 */
void TurretEnemy::think(double delta, const EnemySensor& es){
    // 動かない(砲台)
    hv = 0.0;
    // 発射の時間を減らす
    fireTimer -= delta;

    // 条件：プレイヤーが視界内にいるときだけ撃つ
    if(!es.playerInSight ||
        es.distanceToPlayer > TurretConfig::MAX_FIRE_DISTANCE ||    // プレイヤーとの距離が一定以上
        std::abs(es.dyToPlayer) > TurretConfig::MAX_VERTICAL_DIFF){ // プレイヤーの高さが離れている(dyが一定以上)
        return; // 撃たない
    }
    // 条件を満たしてもすでにtrueなら何もしない→連射抑制
    if(fireRequested){
        return;
    }
    // プレイヤーが視界内にいたら
    if(fireTimer <= 0.0){
        // プレイヤーの左右の向きで発射方向を決める
        fireDir = es.playerOnLeft ? Direction::Left : Direction::Right;
        fireRequested = true;
        fireTimer = TurretConfig::FIRE_INTERVAL;
    }
}

/**
 * @brief 発射要求に応える関数
 * ※PlayingSceneなどがfireRequestedを触らないようにするため
 * 
 * @param outDir 
 * @return true 
 * @return false 
 */
bool TurretEnemy::consumeFireRequest(Direction& outDir) noexcept{
    if(!fireRequested){
        return false;
    }
    outDir = fireDir;
    fireRequested = false;
    return true;
}
