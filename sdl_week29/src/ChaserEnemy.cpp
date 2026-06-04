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
    // 前に進めるか
    bool cannotMove = (!es.groundAhead || es.wallAhead);
    // プレイヤーがいるとき かつ 壁がなければ追いかける
    if(es.playerInSight && cannotMove){
        hv = 0.0;
        return;
    }
    // 接地していない場合はここで返す※空中での高速方向転換を防止
    if(!isOnGround()){
        return;
    }

    // プレイヤーがいないときは歩き回る
    if(!es.playerInSight){
        if(cannotMove){
            dir = (dir == Direction::Right ? Direction::Left : Direction::Right);
        }
        hv = (dir == Direction::Right ? speed : -speed);
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
