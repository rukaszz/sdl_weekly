#include "JumperEnemy.hpp"
#include "JumperConfig.hpp"
#include "EnemySensor.hpp"
#include "Direction.hpp"

#include <random>
#include <cmath>
#include <algorithm>

/**
 * @brief Construct a new Jumper Enemy:: Jumper Enemy object
 * ジャンプのランダム化のために乱数生成器も初期化
 * 
 * @param tex 
 */
JumperEnemy::JumperEnemy(Texture& tex)
    : Enemy(tex), 
      rng(std::random_device{}()), 
      dist(JumperConfig::MIN_INTERVAL, JumperConfig::MAX_INTERVAL)
{

}

/**
 * @brief JumperEnemyの行動決定関数
 * 
 * @param delta 
 * @param es 
 */
void JumperEnemy::think(double delta, const EnemySensor& es){
    // 基本は動かない
    hv = 0.0;

    // クールダウンは常に減らす
    jumpCooldown = std::max(0.0, jumpCooldown - delta);

    // Playerが視界外なら何もしない
    // 壁や崖回避ロジックも実装しない
    if(!es.playerInSight || es.distanceToPlayer > JumperConfig::MIN_TRIGGER_DISTANCE){
        return;
    }

    // ジャンプの条件：地面にいる かつ クールダウンが0.0
    if(isOnGround() && jumpCooldown <= 0.0){
        // 水平方向移動は，Player方向へ少しだけ移動する
        if(es.playerOnLeft){    // Playerが左側
            dir = Direction::Left;
            hv = -speed * moveSpeedScale;
        } else {    // Playerが右側
            dir = Direction::Right;
            hv = speed * moveSpeedScale;
        }
        // 垂直速度のセット(上方向)
        vv = -jumpSpeed;
        // 次のジャンプまでの間隔をランダムに設定
        jumpCooldown = nextInterval();
    } else {
        // 空中 or クールダウン中
        // 横移動などの処理を実装する場合はここ
    }
}

/**
 * @brief 次のジャンプ間隔を乱数生成器を用いて決定する関数
 * 
 * @return double 
 */
double JumperEnemy::nextInterval(){
    return dist(rng);
}

