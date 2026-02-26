#ifndef ENEMYAICONFIG_H
#define ENEMYAICONFIG_H

namespace EnemyAIConfig{
    // 視界
    static inline constexpr double SIGHT_MAX_X = 400.0;
    static inline constexpr double SIGHT_MAX_Y = 96.0;
    // 少し先を調べるための定数
    static inline constexpr double PROBE_WIDTH = 96.0;  // センサーの領域の幅
    static inline constexpr double GROUND_PROBE_DEPTH = 96.0;   // 崖の知覚幅
    static inline constexpr int WALL_PROBE_DEPTH = 4;   // 壁の知覚幅
}   // namespace EnemyAIConfig

#endif  // ENEMYAICONFIG_H