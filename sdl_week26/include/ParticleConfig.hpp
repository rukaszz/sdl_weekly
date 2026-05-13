#ifndef PARTICLECONFIG_H
#define PARTICLECONFIG_H

#include <array>
#include <SDL2/SDL.h>

// Particle用定数
namespace ParticleConfig{
    // 色の設定
    static inline constexpr SDL_Color YELLOW = {255, 220, 50, 255};
    static inline constexpr SDL_Color ORANGE = {255, 120, 0, 255};

    // 拡散方向の設定(int(型)の初期化が必要なので，波括弧は3つ使う)
    // それぞれ正規化済み
    // 斜め4方向
    std::array<std::array<int, 2>, 4> DIRECTION_4 = {{
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    }};
    // 8方向
    std::array<std::array<int, 2>, 8> DIRECTION_8 = {{
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    }};

    // パーティクルの設定値
    struct ParticleMetrics{
        double speed;
        double life;
        int size;
    };
    // coin取得時
    static inline constexpr ParticleMetrics COINSPARK{
        100.0,  // speed 
        0.35,   // life
        5,      // size
    };
    // 敵撃破時
    static inline constexpr ParticleMetrics ENEMYBURST{
        80.0,  // speed 
        0.45,   // life
        6,      // size
    };
}   // namespace ParticleConfig

#endif  // PARTICLECONFIG_H
