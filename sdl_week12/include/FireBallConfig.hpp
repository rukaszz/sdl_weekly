#ifndef FIREBALLCONFIG_H
#define FIREBALLCONFIG_H

struct FireBallConfig{
    static inline constexpr double SPEED_X = 400.0; 
    static inline constexpr double GRAVITY = 800.0;
    static inline constexpr double BOUNCE = 0.6;    // 反発係数
    
    static inline constexpr int FRAME_W = 32;
    static inline constexpr int FRAME_H = 32;
    static inline constexpr int NUM_FRAMES = 4;
    static inline constexpr double ANIM_INTERVAL = 0.08;
};


#endif  // FIREBALLCONFIG_H