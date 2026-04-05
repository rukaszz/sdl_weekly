#ifndef PLAYERCONFIG_H
#define PLAYERCONFIG_H

#include "GameEvent.hpp"

// Player用定数
namespace PlayerConfig{
    static inline constexpr int POS_X = 50;     // 初期位置x
    static inline constexpr int POS_Y = 600;    // 初期位置y
    // スプライト関係の定義
    static inline constexpr int NUM_FRAMES = 8; // スプライトアニメーション枚数
    // プレイヤーの状態に対応するスプライト・当たり判定の情報
    struct FormMetrics{
        int frame_W;
        int frame_H;
        int collisionMargin_X;
        int collisionMargin_Y;
    };
    
    // PlayerForm::Small
    static inline constexpr FormMetrics SMALL_METRICS{
        114,    // frame_W     
        110,    // frame_H
        40,     // collisionMargin_X
        40,     // collisionMargin_Y
    };
    
    // PlayerForm::Super
    static inline constexpr FormMetrics SUPER_METRICS{
        114,    // frame_W     
        150,    // frame_H
        40,     // collisionMargin_X
        24,     // collisionMargin_Y
    };

    // PlayerForm::Fire(Superとサイズは同じ，見た目のみ違う)
    static inline constexpr FormMetrics FIRE_METRICS{
        114,    // frame_W     
        150,    // frame_H
        40,     // collisionMargin_X
        24,     // collisionMargin_Y
    };
    
    static inline constexpr double PLAYER_GRAVITY  = 880.0;     // キャラクタの重力
    static inline constexpr double JUMP_VELOCITY  = 440.0;      // ジャンプ速度
    static inline constexpr double eps  = 1.0;                  // 踏みつけ時の若干の許容範囲ε
    static inline constexpr double COYOTE_TIME = 0.08;          // コヨーテタイム猶予：約5フレーム(0.016*5)
    static inline constexpr double JUMP_HOLD_MAX_TIME = 0.3;    // ジャンプボタン持続押下：300ms程度の猶予
    
    static inline constexpr double RUN_MAX_SPEED = 200.0;       // 最高水平方向速度[px/s]
    static inline constexpr double ACCELERATION = 2000.0;       // 加速度[px/s^2]
    static inline constexpr double FRICTION = 500.0;            // 摩擦係数[px/s^2]
    static inline constexpr double JUMPABLE_BUFFER_TIME = 0.08; // 接地直前でもジャンプ可能となる猶予：約5フレーム(0.016*5)
    
    static inline constexpr double DAMAGE_INVINCIBLE_TIME = 1.0;    // ダメージ時の無敵時間(1秒)

    static inline constexpr double DASH_MAX_SPEED = 340.0;      // ダッシュ時の速度[px/s]
    static inline constexpr double DASH_ACCELERATION = 1400.0;  // ダッシュ加速度[px/s^2]
    static inline constexpr double DASH_FRICTION = 700.0;       // ダッシュ時の摩擦係数[px/s^2]
}   // namespace PlayerConfig

#endif  // PLAYERCONFIG_H
