#ifndef BOSSCONFIG_H
#define BOSSCONFIG_H

namespace BossConfig{    
    static inline constexpr int FRAME_W = 128;          // スプライトの幅
    static inline constexpr int FRAME_H = 128;          // スプライトの高さ
    static inline constexpr int NUM_FRAMES = 10;        // スプライトアニメーション枚数
    static inline constexpr double ANIM_INTERVAL = 0.1; // スプライトアニメーション切り替え秒数
        
    static inline constexpr double RUN_MAX_SPEED = 200.0;   // 最高水平方向速度[px/s]
    static inline constexpr int SCORE_AT_DEATH = 1000;      // 最高水平方向速度[px/s]
    static inline constexpr double FIRE_COOLDOWN = 1.5;     // ファイアボール発射間隔
    static inline constexpr double DAMAGE_INVINCIBLE_TIME = 1.5;    // ダメージ後の無敵時間
    static inline constexpr int CONTACT_DAMAGE = 1;         // 接触時のダメージ
    static inline constexpr int FIREBALL_DAMAGE = 1;        // ファイアボールのダメージ
    // ボスの視界
    static inline constexpr double BOSS_GAZE_X = 600.0;     // X軸
    static inline constexpr double BOSS_GAZE_Y = 96.0;      // Y軸
}   // BossConfig

#endif  // BOSSCONFIG_H
