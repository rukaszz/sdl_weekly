#ifndef ENEMYCONFIG_H
#define ENEMYCONFIG_H

// Enemy用定数
struct EnemyConfig{
    static inline constexpr int FRAME_W = 114;
    static inline constexpr int FRAME_H = 110;
    static inline constexpr double SPEED_MAX = 200.0;
    static inline constexpr double SPEED_MIN = 50.0;
    static inline constexpr int NUM_FRAMES = 8;
    static inline constexpr int SCORE_AT_STOMP = 500;
    static inline constexpr int SCORE_AT_FIREBALL = 100;
    static inline constexpr double DYING_DURATION = 1.5;    // 秒
    static inline constexpr double DYING_BLINK_INTERVAL = 0.1;  // 秒
};

#endif  // ENEMYCONFIG_H
