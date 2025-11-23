#ifndef GAME_H
#define GAME_H
#include "Window.hpp"
#include "Renderer.hpp"
#include "Player.hpp"

class Game{
private:
    Window window;
    Renderer renderer;
    Player player;
    bool running;

public:
    // 定数
    static inline constexpr int TARGET_FPS = 60;
    static inline constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    Game();
    void run();

private:
    void processEvents();
    void capFrameRate(Uint32 nowTime);
    void update(double delta);
    void render();

};

#endif  // GAME_H