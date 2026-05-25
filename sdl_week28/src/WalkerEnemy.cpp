#include "WalkerEnemy.hpp"

#include "Direction.hpp"
#include "EnemySensor.hpp"

WalkerEnemy::WalkerEnemy(Texture& tex)
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
void WalkerEnemy::think(double delta, const EnemySensor& es){
    // Walkerは使わないのでdeltaは停止
    (void)delta;
    // 判定
    if(!es.groundAhead || es.wallAhead){
        // 崖や壁があるなら方向転換
        if(dir == Direction::Right){
            dir = Direction::Left;
        } else {
            dir = Direction::Right;
        }
    }
    // dirに応じてhvを設定する
    hv = (dir == Direction::Right ? speed : -speed);
}
