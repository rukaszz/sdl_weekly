#include "ChaserEnemy.hpp"

#include "Direction.hpp"
#include "EnemySensor.hpp"

ChaserEnemy::ChaserEnemy(Texture& tex)
    : Enemy(tex)
{

}

/**
 * @brief 敵の移動などの処理の前に現状を把握し行動の判断を実施する
 * Walkerの場合は目の前に崖/壁があるときに方向転換する
 * 
 * @param delta 
 * @param es 
 */
void ChaserEnemy::think(double /*delta*/, const EnemySensor& es){
    
    if(!es.playerInSight){
        // hv = 0.0;    // プレイヤーが見えていないなら何もしない
        // プレイヤーがいないときは歩き回る
        if(!es.groundAhead || es.wallAhead){
            dir = (dir == Direction::Right ? Direction::Left : Direction::Right);
        }
        hv = (dir == Direction::Right ? speed : -speed);
        return;
    }

    // 近づきすぎたら追いかけを止める(仮実装)
    constexpr double MIN_CHASE_DISTANCE = 1.0;
    if(es.distanceToPlayer < MIN_CHASE_DISTANCE){
        hv = 0.0;
        return;
    }

    // プレイヤーのいる位置を見て追跡方向を決定する
    if(es.playerOnLeft){
        dir = Direction::Left;
        hv = -speed;
    } else {
        dir = Direction::Right;
        hv = speed;
    }
}
