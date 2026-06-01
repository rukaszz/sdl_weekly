#ifndef BOSSBATTLESTATE_H
#define BOSSBATTLESTATE_H

/**
 * @brief ボスの状態管理用
 * 
 */
enum class BossBattlePhase{
    None, 
    WaitingTrigger, 
    Active, 
    Defeated, 
};

/**
 * @brief ボス戦に関する状態
 * 構造体だが，簡単な状態確認用の関数は定義する
 * 
 */
struct BossBattleState{
    // ボスの状態
    BossBattlePhase phase = BossBattlePhase::None;

    double trigger_X = 0.0;     // ボス戦開始位置
    double cameraMin_X = 0.0;   // ボス戦中のステージの左端
    double cameraMax_X = 0.0;   // ボス戦中のステージの右端
    
    // 状態確認用関数
    // ボス戦があるか
    bool hasBoss() const noexcept{
        // Noneではない=そのステージにボスがある
        return phase != BossBattlePhase::None;
    }
    // トリガー待機中
    bool isWaiting() const noexcept{
        return phase == BossBattlePhase::WaitingTrigger;
    }
    // ボス戦中か
    bool isActive() const noexcept{
        return phase == BossBattlePhase::Active;
    }
    // ボスが負けたか(=ボス戦終了か)
    bool isDefeated() const noexcept{
        return phase == BossBattlePhase::Defeated;
    }
};

#endif  // BOSSBATTLESTATE_H
