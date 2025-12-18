// 定数
#include "GameConfig.hpp"
#include "PlayerConfig.hpp"
#include "EnemyConfig.hpp"

#include "Window.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "TitleScene.hpp"
#include "PlayingScene.hpp"
#include "GameOverScene.hpp"
#include "Texture.hpp"
#include "Character.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Game.hpp"
#include "GameUtil.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"
#include "Input.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>

void Game::initSDL(){
    // SDL初期化
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        throw std::runtime_error(SDL_GetError());
    }
    // SDL_IMG初期化
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        throw std::runtime_error(IMG_GetError());
    }
    if((TTF_Init() != 0)){
        throw std::runtime_error(SDL_GetError());
    }
}

/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game(){
    // 初期化
    initSDL();
    // ウィンドウ
    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    // レンダラー
    renderer = std::make_unique<Renderer>(window->get());
    
    // プレイヤー
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    player = std::make_unique<Player>(*playerTexture);
    player->setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 敵
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    auto enemyTex = enemyTexture.get();
    for(int i = 0;i < 5; ++i){
        enemies.push_back(std::make_unique<Enemy>(*enemyTex));
    }

    // テキスト
    font = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    // スコア
    scoreText = std::make_unique<TextTexture>(*renderer, font.get(), SDL_Color{255, 255, 255, 255});
    scoreText->setText("Score: 0");
    // FPS
    fpsText = std::make_unique<TextTexture>(*renderer, font.get(), SDL_Color{255, 255, 255, 255});
    fpsText->setText("");
    // input抽象化
    input = std::make_unique<Input>();
    
    running = true;

    // 乱数
    distX = std::uniform_real_distribution<double>(GameConfig::WINDOW_WIDTH/2, GameConfig::WINDOW_WIDTH - EnemyConfig::FRAME_W);
    distY = std::uniform_real_distribution<double>(EnemyConfig::FRAME_H, GameConfig::WINDOW_HEIGHT - EnemyConfig::FRAME_H);
    distSpeed = std::uniform_real_distribution<double>(EnemyConfig::SPEED_MIN, EnemyConfig::SPEED_MAX);

    // シーン
    ctx = std::make_unique<GameContext>(GameContext{
        *renderer,
        *player,
        enemies,
        *scoreText,
        *fpsText,
        rd,
        distX,
        distY,
        distSpeed,
        font.get()
    });
    scenes[(int)GameScene::Title]   = std::make_unique<TitleScene>(*this, *ctx);
    scenes[(int)GameScene::Playing] = std::make_unique<PlayingScene>(*this, *ctx);
    scenes[(int)GameScene::GameOver]= std::make_unique<GameOverScene>(*this, *ctx);

    currentScene = scenes[(int)GameScene::Title].get();
    currentScene->onEnter();
}

/**
 * @brief Destroy the Game:: Game object
 * 
 */
Game::~Game(){
    /*
     * Gameオブジェクトが破棄されたあとにSDLの終了処理を実施する
     * →リーク回避のため
     * DBusのリークはSDL_Linux側の問題なので無視
     */
    // quitSDL();
}

/**
 * @brief シーンの切り替えを実施する関数
 * 抽象クラスSceneに対して，ポリモーフィズムによる派生クラス生成を実施する
 * 
 * @param s 
 */
void Game::changeScene(GameScene id){
    currentScene->onExit();
    currentScene = scenes[(int)id].get();
    currentScene->onEnter();
}

/**
 * @brief ゲームの状態をリセットする関数
 * Playerは初期位置
 * Enemyはランダムに配置，スピードも変化
 * animationControllerのフレームのリセット
 * scoreもリセット
 * 
 */
void Game::resetGame(){
    score = 0;
    player->reset();
    for(auto& e : enemies){
        e->reset(rd, distX, distY, distSpeed);
    }
}

/**
 * @brief ゲーム実行用関数
 * 
 */
void Game::run(){
    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    
    // FPS 計測用（これはミリ秒で十分）
    Uint32 fpsTimer = SDL_GetTicks();
    int fpsCounter = 0;

    while(running){
        // 時間の計測
        Uint64 nowCounter = SDL_GetPerformanceCounter();
        double delta = static_cast<double>(nowCounter - lastCounter) / static_cast<double>(frequency);
        lastCounter = nowCounter;
        // クランプ：deltaが大きすぎて更新しすぎることを防ぐ
        if(delta > 0.1){
            delta = 0.1;    // 10FPSくらい
        }
        // 各種処理の実施
        processEvents();
        update(delta);
        render();
        // フレームレートの計算とfps計測
        ++fpsCounter;
        Uint32 nowMs = SDL_GetTicks();
        if(nowMs - fpsTimer >= 1000){
            fpsText->setText("FPS: " + std::to_string(fpsCounter));
            fpsCounter = 0;
            fpsTimer = nowMs;
        }
        // fpsキャップ(最大60fps)
        capFrameRate(nowMs);
    }
}

/**
 * @brief fpsキャップを実装(fpsを固定化するため)
 * 
 * @param frameStartMs: 計測点
 * @return * void 
 */
void Game::capFrameRate(Uint32 frameStartMs){
    Uint32 frameDuration = SDL_GetTicks() - frameStartMs;
    if(frameDuration < Game::FRAME_DELAY){
        SDL_Delay(Game::FRAME_DELAY - frameDuration);
    }
}

/**
 * @brief ポーリング処理によるイベント管理
 * 
 */
void Game::processEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            running = false;
        }
        input->handleEvent(e);
        currentScene->handleEvent(e);
    }
}

/**
 * @brief ゲーム更新関数
 * 
 * @param delta: 前フレームとの差分
 */
void Game::update(double delta){
    currentScene->update(delta);
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer->clear();
    currentScene->render();
    renderer->present();
}



