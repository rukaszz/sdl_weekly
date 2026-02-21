#ifndef JUMPERCONFIG_H
#define JUMPERCONFIG_H

// JumperEnemy用定数
struct JumperConfig{
    static inline constexpr double MIN_INTERVAL = 1.0;   
    static inline constexpr double MAX_INTERVAL = 2.0;   
    static inline constexpr double JUMP_SPEED = 800.0;              // 垂直速度
    static inline constexpr double MOVE_SPEED_SCALE = 0.3;          // 横方向の速度    
    static inline constexpr double MIN_TRIGGER_DISTANCE = 150.0;    // Playerを見つける距離
};

#endif  // JUMPERCONFIG_H
