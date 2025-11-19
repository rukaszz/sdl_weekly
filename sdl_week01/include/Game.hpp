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
    double fps;

public:
    Game();
    void run();

private:
    void processEvents();
    void update(double delta);
    void render();

};

#endif  // GAME_H