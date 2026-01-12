#ifndef GAME_H
#define GAME_H

#include "Window.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"
#include "GameContext.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>
#include <random>

// ゲームの状態管理
enum class GameScene{
    Title, 
    Playing, 
    GameOver, 
    Clear, 
    Count, 
};

// 世界の広さ(ステージの広さ)
struct WorldInfo{
    double WorldWidth;
    double WorldHeight;
};

class Game : public SceneControl{
private:
    // スマートポインタ
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    // シーン管理
    std::unique_ptr<Scene> scenes[static_cast<int>(GameScene::Count)];
    Scene* currentScene = nullptr;
    // Characterオブジェクト
    std::unique_ptr<Texture> playerTexture;
    std::unique_ptr<Player> player;
    std::unique_ptr<Texture> enemyTexture;
    std::vector<std::unique_ptr<Enemy>> enemies;
    // ブロック
    std::vector<Block> blocks;
    // テキスト表示用
    std::unique_ptr<Text> font;
    std::unique_ptr<TextTexture> titleText;
    std::unique_ptr<TextTexture> gameTitleText;
    std::unique_ptr<TextTexture> scoreText;
    std::unique_ptr<TextTexture> fpsText;
    std::unique_ptr<TextTexture> gameOverText;
    // input抽象化
    std::unique_ptr<Input> input;
    // カメラ
    Camera camera;
    // ステージ(世界)の情報
    WorldInfo worldInfo;

    // 変数系
    bool running = true;
    GameScene scene = GameScene::Title;
    // 仮のスコア(生存時間=スコアになる簡易的なもの)
    uint32_t score = 0;
    // メルセンヌツイスタ
    std::mt19937 rd{std::random_device{}()};
    // 乱数(x, y座標)
    std::uniform_real_distribution<double> distX;
    std::uniform_real_distribution<double> distY;
    // speed
    std::uniform_real_distribution<double> distSpeed;
    // シーン管理用
    std::unique_ptr<GameContext> ctx;

public:
    // 定数
    static inline constexpr int TARGET_FPS = 60;
    static inline constexpr double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;
    static inline constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    Game();
    ~Game();
    void run();
    void changeScene(GameScene id) override;
    void resetGame() override;

private:
    void initSDL();
    void processEvents();
    void capFrameRate(Uint32 frameStartMs);
    void update(double delta);
    void render();
    void displayText(const std::string& dispStr, const std::string& color);

public:
    // getters
    uint32_t getScore() override{
        return score;
    }
    // setters
    void setScore(uint32_t v) override{
        score = v;
    }
};

#endif  // GAME_H
