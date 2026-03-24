#ifndef BOSSBATTLESTATE_H
#define BOSSBATTLESTATE_H

struct BossBattleState{
    bool hasBoss = false;       // このステージにボス戦があるか
    bool active = false;        // ボス戦が開始したか
    bool isBossDefeated = false;// ボスを撃破したか
    double trigger_X = 0.0;     // ボス戦開始位置
    double cameraMin_X = 0.0;   // ボス戦中のステージの左端
    double cameraMax_X = 0.0;   // ボス戦中のステージの右端
    // int bossPhase = 0;   // ボスの形態
};

#endif  // BOSSBATTLESTATE_H
