#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// Game用定数
struct GameConfig{
    // Windowサイズ
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 800;
    
    // Character系
    static constexpr int PLAYER_START_X = 100;
    static constexpr int PLAYER_START_Y = 250;

    static constexpr int ENEMY1_START_X = 50;
    static constexpr int ENEMY1_START_Y = 50;

    static constexpr int ENEMY2_START_X = 250;
    static constexpr int ENEMY2_START_Y = 650;

    // 得点用係数
    static constexpr double SCORE_RATE = 100.0;

};

#endif  // GAMECONFIG_H
