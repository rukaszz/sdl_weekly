#ifndef GAME_H
#define GAME_H

#include "Window.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"

#include <memory>
#include <vector>

// ゲームの状態管理
enum class GameState{
    Title, 
    Playing, 
    GameOver, 
};

class Game{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    // Characterオブジェクト
    std::unique_ptr<Texture> playerTexture;
    std::unique_ptr<Player> player;
    std::unique_ptr<Texture> enemyTexture;
    std::vector<std::unique_ptr<Enemy>> enemies;
    // テキスト表示用
    std::unique_ptr<Text> text; 
    std::unique_ptr<TextTexture> titleText;
    std::unique_ptr<TextTexture> gameTitleText;
    std::unique_ptr<TextTexture> scoreText;
    std::unique_ptr<TextTexture> gameOverText;

    bool running = true;
    GameState state = GameState::Title;
    // 仮のスコア(生存時間=スコアになる簡易的なもの)
    uint32_t score = 0;

public:
    // 定数
    static inline constexpr int TARGET_FPS = 60;
    static inline constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    Game();
    ~Game();
    void run();

private:
    void initSDL();
    void quitSDL();
    void shutdown();
    void processEvents();
    void capFrameRate(Uint32 nowTime);
    void update(double delta);
    void render();
    void reset();
    void displayText(const std::string& dispStr, const std::string& color);

};

#endif  // GAME_H