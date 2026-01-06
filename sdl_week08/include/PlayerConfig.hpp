#ifndef PLAYERCONFIG_H
#define PLAYERCONFIG_H

// Player用定数
struct PlayerConfig{
    static inline constexpr int POS_X = 50;
    static inline constexpr int POS_Y = 600;
    static inline constexpr int FRAME_W = 114;
    static inline constexpr int FRAME_H = 110;
    static inline constexpr int NUM_FRAMES = 8;
    static inline constexpr double PLAYER_GRAVITY  = 880.0;
    static inline constexpr double JUMP_VELOCITY  = 660.0;
};

#endif  // PLAYERCONFIG_H
