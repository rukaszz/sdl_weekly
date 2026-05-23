#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// Game用定数
namespace GameConfig{
    // Windowサイズ
    static inline  constexpr int WINDOW_WIDTH = 800;
    static inline  constexpr int WINDOW_HEIGHT = 800;

    // 得点用係数
    static inline  constexpr double SCORE_RATE = 100.0;
    // GameOver時のペナルティ
    static inline constexpr int SCORE_PENALTY = -5000.0;

    // デバッグ用の制御用変数
    // fpsなどの情報の制御用
    static inline  constexpr bool SHOW_DEBUG_TEXT = true;

}   // namespace GameConfig

#endif  // GAMECONFIG_H
