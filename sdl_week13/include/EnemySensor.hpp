#ifndef ENEMYSENSOR_H
#define ENEMYSENSOR_H

/**
 * @brief AIが判断に用いるGameの情報
 * 
 */
struct EnemySensor{
    double distanceToPlayer;    // プレイヤーとの距離
    bool playerOnLeft;          // プレイヤーが左にいるか
    bool playerInSight;         // プレイヤーが敵の見える範囲にいるか
    bool groundAhead;           // 一歩先に足場があるか(無いなら次の一歩で落ちる)
    bool wallAhead;             // 一歩先に壁があるか
    bool playerBelow;           // プレイヤーが下にいるか
};

#endif  // ENEMYSENSOR_H