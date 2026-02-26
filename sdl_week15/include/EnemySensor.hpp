#ifndef ENEMYSENSOR_H
#define ENEMYSENSOR_H

#include <vector>

struct Block;

/**
 * @brief EnemyのAIが行動を判断するためのプレイヤー情報
 * 
 */
struct PlayerInfo{
    double center_X;    // プレイヤーの中心座標x
    double center_Y;    // プレイヤーの中心座標y
};

/**
 * @brief 敵の行動を決定するためのEnemy外部の情報
 * gatherEnemySensorで使用する
 * 
 */
struct EnemySensorContext{
    const std::vector<Block>& blocks;
    double worldWidth;
    PlayerInfo playerInfo;
};

/**
 * @brief AIが判断に用いるGameの情報
 * 
 */
struct EnemySensor{
    double distanceToPlayer = 0.0;  // プレイヤーとの距離
    double dxToPlayer = 0.0;        // プレイヤーとのx軸距離
    double dyToPlayer = 0.0;        // プレイヤーとのy軸距離
    bool playerOnLeft = false;      // プレイヤーが左にいるか
    bool playerInSight = false;     // プレイヤーが敵の見える範囲にいるか
    bool groundAhead = false;       // 一歩先に足場があるか(無いなら次の一歩で落ちる)
    bool wallAhead = false;         // 一歩先に壁があるか
    bool playerBelow = false;       // プレイヤーが下にいるか
};

#endif  // ENEMYSENSOR_H