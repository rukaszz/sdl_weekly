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

#include <memory>
#include <vector>
#include <random>

// ゲームの状態管理
enum class GameScene{
    Title, 
    Playing, 
    GameOver, 
};

class Game{
private:
    // スマートポインタ
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    // シーン管理
    std::unique_ptr<Scene> currentScene;
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
    std::unique_ptr<TextTexture> fpsText;
    std::unique_ptr<TextTexture> gameOverText;

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
    // タイトル処理
    double titleFade = 0.0; // 0.0 .. 1.0
    double blinkTimer = 0.0;
    bool blinkVisible= true;

public:
    // 定数
    static inline constexpr int TARGET_FPS = 60;
    static inline constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    Game();
    ~Game();
    void run();
    void changeScene(GameScene s);
    void updateTitle(double delta);
    void reset();

private:
    void initSDL();
    void processEvents();
    void capFrameRate(Uint32 nowTime);
    void update(double delta);
    void render();
    void displayText(const std::string& dispStr, const std::string& color);

public:
    // getters
    Window& getWindow() const{
        return *window;
    }
    Renderer& getRenderer() const{
        return *renderer;
    }
    Player& getPlayer() const{
        return *player;
    }
    std::vector<std::unique_ptr<Enemy>>& getEnemies(){
        return enemies;
    }
    TextTexture& getScoreText(){
        return *scoreText;
    }
    TextTexture& getFpsText(){
        return *fpsText;
    }
    TextTexture& getTitleText(){
        return *titleText;
    }
    TextTexture& getGameTitleText(){
        return *gameTitleText;
    }
    TextTexture& getGameOverText(){
        return *gameOverText;
    }
    uint32_t getScore(){
        return score;
    }
    std::mt19937& getRandom(){
        return rd;
    }
    // setters
    void setScore(uint32_t v){
        score = v;
    }

};

#endif  // GAME_H