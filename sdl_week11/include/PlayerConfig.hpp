#ifndef PLAYERCONFIG_H
#define PLAYERCONFIG_H

// Player用定数
struct PlayerConfig{
    static inline constexpr int POS_X = 50;     // 初期位置x
    static inline constexpr int POS_Y = 600;    // 初期位置y
    static inline constexpr int FRAME_W = 114;  // スプライトの幅
    static inline constexpr int FRAME_H = 110;  // スプライトの高さ
    static inline constexpr int NUM_FRAMES = 8; // スプライトアニメーション枚数
    static inline constexpr double PLAYER_GRAVITY  = 880.0; // キャラクタの重力
    static inline constexpr double JUMP_VELOCITY  = 440.0;  // ジャンプ速度
    static inline constexpr double eps  = 1.0;    // 踏みつけ時の若干の許容範囲ε
    static inline constexpr double COYOTE_TIME = 0.08;  // コヨーテタイム猶予：約5フレーム(0.016*5)
    static inline constexpr double JUMP_HOLD_MAX_TIME = 0.3;    // ジャンプボタン持続押下：300ms程度の猶予
    static inline constexpr double RUN_MAX_SPEED = 200.0;    // 最高水平方向速度[px/s]
    static inline constexpr double ACCELERATION = 2000.0;    // 加速度[px/s^2]
    static inline constexpr double FRICTION = 500.0;    // 摩擦係数[px/s^2]
    static inline constexpr double JUMPABLE_BUFFER_TIME = 0.08;  // 接地直前でもジャンプ可能となる猶予：約5フレーム(0.016*5)
};

#endif  // PLAYERCONFIG_H
