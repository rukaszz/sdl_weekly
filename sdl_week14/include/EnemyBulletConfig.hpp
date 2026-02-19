#ifndef ENEMYBULLETCONFIG_H
#define ENEMYBULLETCONFIG_H

struct EnemyBulletConfig{
    static inline constexpr double SPEED_X = 400.0; 
    
    static inline constexpr int FRAME_W = 32;
    static inline constexpr int FRAME_H = 32;
    static inline constexpr int NUM_FRAMES = 4;
    static inline constexpr double ANIM_INTERVAL = 0.08;
};


#endif  // ENEMYBULLETCONFIG_H