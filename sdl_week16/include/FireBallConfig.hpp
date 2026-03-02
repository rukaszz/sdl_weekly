#ifndef FIREBALLCONFIG_H
#define FIREBALLCONFIG_H

namespace FireBallConfig{
    static inline constexpr double SPEED_X = 400.0; 
    static inline constexpr double GRAVITY = 800.0;
    static inline constexpr double BOUNCE = 0.8;    // 反発係数
    static inline constexpr int MAX_BOUNCE = 5;     // 最大バウンド回数(超えたら消す)
    static inline constexpr int SPAWN_OFFSET_Y = 5; // ファイアボールの出現位置調整用
    
    static inline constexpr int FRAME_W = 32;
    static inline constexpr int FRAME_H = 32;
    static inline constexpr int NUM_FRAMES = 4;
    static inline constexpr double ANIM_INTERVAL = 0.08;
}   // namespace FireBallConfig

#endif  // FIREBALLCONFIG_H
