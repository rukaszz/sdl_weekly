#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// Game用定数
struct GameConfig{
    // Windowサイズ
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 800;

    // 得点用係数
    static constexpr double SCORE_RATE = 100.0;

};

#endif  // GAMECONFIG_H
