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
    // シーン
    // currentScene = std::make_unique<TitleScene>(*this);
    scenes[(int)GameScene::Title] = std::make_unique<TitleScene>(*this);
    scenes[(int)GameScene::Playing] = std::make_unique<PlayingScene>(*this);
    scenes[(int)GameScene::GameOver] = std::make_unique<GameOverScene>(*this);
    currentScene = scenes[(int)GameScene::Title].get();
    currentScene->onEnter();
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
    text = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    // プロンプト
    titleText = std::make_unique<TextTexture>(renderer->get(), text.get(), SDL_Color{255, 255, 255, 255});
    titleText->setText("Press ENTER to Start");
    // タイトル
    gameTitleText = std::make_unique<TextTexture>(renderer->get(), text.get(), SDL_Color{255, 255, 255, 255});
    gameTitleText->setText("My Game");
    // スコア
    scoreText = std::make_unique<TextTexture>(renderer->get(), text.get(), SDL_Color{255, 255, 255, 255});
    scoreText->setText("Score: 0");
    // FPS
    fpsText = std::make_unique<TextTexture>(renderer->get(), text.get(), SDL_Color{255, 255, 255, 255});
    fpsText->setText("");
    // ゲームオーバー
    gameOverText = std::make_unique<TextTexture>(renderer->get(), text.get(), SDL_Color{255, 255, 255, 255});
    gameOverText->setText("GameOver");
    running = true;

    // 乱数
    distX = std::uniform_real_distribution<double>(GameConfig::WINDOW_WIDTH/2, GameConfig::WINDOW_WIDTH - EnemyConfig::FRAME_W);
    distY = std::uniform_real_distribution<double>(EnemyConfig::FRAME_H, GameConfig::WINDOW_HEIGHT - EnemyConfig::FRAME_H);
    distSpeed = std::uniform_real_distribution<double>(EnemyConfig::SPEED_MIN, EnemyConfig::SPEED_MAX);
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
 * @brief ゲーム実行用関数
 * 
 */
void Game::run(){
    Uint32 lastTime = SDL_GetTicks();
    Uint32 fpsTimer = SDL_GetTicks();
    int fpsCounter = 0;

    while(running){
        // 時間の計測
        Uint32 nowTime = SDL_GetTicks();
        double delta = (nowTime - lastTime) * 0.001;
        lastTime = nowTime;
        // 各種処理の実施
        processEvents();
        update(delta);
        render();
        // フレームレートの計算とfps計測
        ++fpsCounter;
        if(nowTime - fpsTimer >= 1000){
            fpsText->setText("FPS: " + std::to_string(fpsCounter));
            fpsCounter = 0;
            fpsTimer = nowTime;
        }
        // fpsキャップ(最大60fps)
        capFrameRate(nowTime);
    }
}

/**
 * @brief シーンの切り替えを実施する関数
 * 抽象クラスSceneに対して，ポリモーフィズムによる派生クラス生成を実施する
 * 
 * @param s 
 */
void Game::changeScene(GameScene s){
    currentScene->onExit();
    currentScene = scenes[(int)s].get();
    currentScene->onEnter();
}

/**
 * @brief fpsキャップを実装(fpsを固定化するため)
 * 
 * @param fps_nowTime: 計測点
 */
void Game::capFrameRate(Uint32 frameStart){
    Uint32 frame_duration = SDL_GetTicks() - frameStart;
    
    if(frame_duration < Game::FRAME_DELAY){
        SDL_Delay(Game::FRAME_DELAY - frame_duration);
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

/**
 * @brief ゲームの状態をリセットする関数
 * Playerは初期位置
 * Enemyはランダムに配置，スピードも変化
 * animationControllerのフレームのリセット
 * scoreもリセット
 * 
 */
void Game::reset(){
    score = 0;
    player->reset();
    for(auto& e : enemies){
        e->reset(rd, distX, distY, distSpeed);
    }
}

