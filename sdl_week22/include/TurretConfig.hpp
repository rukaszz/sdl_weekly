#ifndef TURRETCONFIG_H
#define TURRETCONFIG_H

// TurretEnemy用定数
namespace TurretConfig{
    static inline constexpr double FIRE_INTERVAL = 1.5; // 射撃間隔(秒)   
    static inline constexpr double MAX_FIRE_DISTANCE = 600.0;   // 射程
    static inline constexpr double MAX_VERTICAL_DIFF = 64.0;    // 射程距離(高さの許容差)
    static inline constexpr double MUZZLE_OFFSET_X = 12.0;       // 発射位置調整用(X軸)
    static inline constexpr double MUZZLE_OFFSET_Y = -6.0;      // 発射位置調整用(Y軸)
};  // namespace TurretConfig

#endif  // TURRETCONFIG_H
