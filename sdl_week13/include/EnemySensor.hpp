#ifndef ENEMYSENSOR_H
#define ENEMYSENSOR_H

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