#ifndef GAME_H
#define GAME_H

#include "GameScene.hpp"

#include "SdlSystem.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "FireBall.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"
#include "GameContext.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>
#include <random>
#include <optional>
#include <array>

class Game : public SceneControl{
private:
    // スマートポインタ
    // RAIIに従い必ず最初にSdlSystemを定義する→最後に呼ばれてSDLが全リソースを解放後に終了される
    std::unique_ptr<SdlSystem> sdl;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    // シーン管理
    // std::unique_ptr<Scene> scenes[static_cast<int>(GameScene::Count)];
    std::array<std::unique_ptr<Scene>, static_cast<size_t>(GameScene::Count)> scenes{};
    std::optional<GameScene> pendingSceneChange;
    Scene* currentScene = nullptr;
    // Characterオブジェクト
    std::unique_ptr<Texture> playerTexture;
    std::unique_ptr<Player> player;
    std::unique_ptr<Texture> enemyTexture;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<Texture> fireballTexture;
    std::vector<std::unique_ptr<FireBall>> fireballs;
    // ブロック
    std::vector<Block> blocks;
    // テキスト表示用
    std::unique_ptr<Text> font;
    std::unique_ptr<TextTexture> scoreText;
    std::unique_ptr<TextTexture> fpsText;
    // input抽象化
    std::unique_ptr<Input> input;
    // カメラ
    Camera camera;
    // ステージ(世界)の情報
    WorldInfo worldInfo;

    // 変数系
    bool running = true;
    // GameScene scene = GameScene::Title;
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

    void requestScene(GameScene id) override;

private:
    // ↓コンストラクタ内のリソース確保関数↓
    void bootstrapWindowAndRenderer();
    void loadResources();
    void buildWorld();
    void buildContexts();
    void buildScenes();
    void startFromTitle();
    // ↑コンストラクタ内のリソース確保関数↑
    void processEvents();
    void capFrameRate(Uint32 frameStartMs);
    void update(double delta);
    void render();
    void displayText(const std::string& dispStr, const std::string& color);
    void applySceneChangeIfAny();
public:
    // シーンのindex取得関数
    static constexpr size_t nextSceneIndex(GameScene s) noexcept{
        return static_cast<size_t>(s);
    }
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
