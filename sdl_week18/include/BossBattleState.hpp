#ifndef BOSSBATTLESTATE_H
#define BOSSBATTLESTATE_H

struct BossBattleState{
    bool active = false;
    double cameraMinX = 0.0;
    double cameraMaxX = 0.0;
    // int bossPhase = 0;   // ボスの形態
    // bool bossDefeated = false;   // ボスに勝ったか
};

#endif  // BOSSBATTLESTATE_H
