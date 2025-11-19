#ifndef GAME_H
#define GAME_H
#include "Window.hpp"
#include "Renderer.hpp"
#include "Player.hpp"

struct GameConst{
    static const int TARGET_FPS;
    static const int FRAME_DELAY;
};

class Game{
private:
    Window window;
    Renderer renderer;
    Player player;
    bool running;

public:
    Game();
    void run();

private:
    void processEvents();
    void fpsCap(double fps_timer_start);
    void update(double delta);
    void render();

};

#endif  // GAME_H