#ifndef GAME_H
#define GAME_H

#include "Window.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

#include <memory>
#include <vector>

// ゲームの状態管理
enum class GameState{
    Playing, 
    GameOver, 
};

class Game{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Texture> playerTexture;
    std::unique_ptr<Player> player;
    std::unique_ptr<Texture> enemyTexture;
    std::vector<std::unique_ptr<Enemy>> enemies;
    bool running;
    GameState state = GameState::Playing;

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
    void reset();

};

#endif  // GAME_H