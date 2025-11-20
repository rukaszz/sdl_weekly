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
    static inline constexpr int TAEGET_FPS = 60;
    static inline constexpr int FRAME_DELAY = 1000 / TAEGET_FPS;

    Game();
    void run();

private:
    void processEvents();
    void fpsFrameRate(Uint32 fps_timer_start);
    void update(double delta);
    void render();

};

#endif  // GAME_H