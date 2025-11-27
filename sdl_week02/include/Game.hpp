#ifndef GAME_H
#define GAME_H
#include "Window.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include <memory>

class Game{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Texture> playerTexture;
    std::unique_ptr<Player> player;
    bool running;

public:
    // 定数
    static inline constexpr int TARGET_FPS = 60;
    static inline constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void capFrameRate(Uint32 nowTime);
    void update(double delta);
    void render();

};

#endif  // GAME_H