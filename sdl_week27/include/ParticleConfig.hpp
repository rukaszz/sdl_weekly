#ifndef PARTICLECONFIG_H
#define PARTICLECONFIG_H

#include <array>
#include <cstddef>
#include <numbers>

#include <SDL2/SDL.h>

// Particle用定数
namespace ParticleConfig{
    // 色の設定
    static inline constexpr SDL_Color YELLOW = {255, 220, 50, 255};
    static inline constexpr SDL_Color ORANGE = {255, 120, 0, 255};
    static inline constexpr SDL_Color RED    = {230, 50, 50, 255};
    static inline constexpr SDL_Color BROWN  = {134, 74, 43, 255};

    // 定数として1/sqrt(2)を定義
    // C++20以降
    // static inline constexpr double INV_SQRT2 = std::numbers::sqrt2 / 2.0;
    // C++20より前はリテラルで定義
    static inline constexpr double INV_SQRT2 = 0.7071067811865476;
    // 画面に存在できるパーティクルの最大数
    static inline constexpr std::size_t MAX_PARTICLES = 256;

    // 拡散方向の設定(int(型)の初期化が必要なので，波括弧は3つ使う)
    // それぞれ正規化
    // 斜め方向はx, yがそれぞれ1だとsqrt(2)倍速になるので，1/sqrt(2)で正規化する
    // 斜め4方向(SDLは下方向がy軸の正になることに注意)
    static inline constexpr std::array<std::array<double, 2>, 4> DIRECTION_4 = {{
        {INV_SQRT2, INV_SQRT2},     // 右下
        {-INV_SQRT2, INV_SQRT2},    // 左下
        {INV_SQRT2, -INV_SQRT2},    // 右上
        {-INV_SQRT2, -INV_SQRT2}    // 左上
    }};
    // 8方向
    static inline constexpr std::array<std::array<double, 2>, 8> DIRECTION_8 = {{
        {1.0, 0.0}, {-1.0, 0.0},    // 右，左
        {0.0, 1.0}, {0.0, -1.0},    // 上，下
        {INV_SQRT2, INV_SQRT2},     // 右下
        {-INV_SQRT2, INV_SQRT2},    // 左下
        {INV_SQRT2, -INV_SQRT2},    // 右上
        {-INV_SQRT2, -INV_SQRT2}    // 左上
    }};

    // パーティクルの設定値
    struct ParticleMetrics{
        double speed;
        double life;
        int size;
    };
    // coin取得時
    static inline constexpr ParticleMetrics COIN_SPARK{
        100.0,  // speed 
        0.65,   // life
        5,      // size
    };
    // 敵撃破時
    static inline constexpr ParticleMetrics ENEMY_BURST{
        100.0,   // speed 
        1.00,   // life
        6,      // size
    };
    // プレイヤー死亡時(少し大きく早い)
    static inline constexpr ParticleMetrics PLAYER_DEATH{
        120.0,  // speed 
        0.7,    // life
        7,      // size
    };
    // ブロック破壊時
    static inline constexpr ParticleMetrics BLOCK_DEBRIS{
        120.0,  // speed 
        0.7,    // life
        7,      // size
    };
}   // namespace ParticleConfig

#endif  // PARTICLECONFIG_H
