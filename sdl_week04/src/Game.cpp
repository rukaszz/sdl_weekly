// 定数
#include "GameConfig.hpp"
#include "PlayerConfig.hpp"
#include "EnemyConfig.hpp"

#include "Window.hpp"
#include "Renderer.hpp"
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

    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    renderer = std::make_unique<Renderer>(window->get());
    
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    player = std::make_unique<Player>(*playerTexture);
    player->setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    
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
    }
}

/**
 * @brief ゲーム更新関数
 * 
 * @param delta: 前フレームとの差分
 */
void Game::update(double delta){
    /* ----- GameOver状態 ----- */
    if(state == GameState::Title){
        // タイトルのフェードインなど
        updateTitle(delta);
        const Uint8* k = SDL_GetKeyboardState(NULL);
        if(k[SDL_SCANCODE_RETURN]){
            state = GameState::Playing;
            score = 0;
            reset();
        }
        return;   // 画面のオブジェクトの更新はしない
    }
    if(state == GameState::GameOver){
        const Uint8* k = SDL_GetKeyboardState(NULL);
        if(k[SDL_SCANCODE_RETURN]){
            reset();    // PlayerとEnemyを元の位置へ戻す
            state = GameState::Playing;
            
        }
        return;
    }
    /* ----- Playing状態 ----- */
    // ウィンドウサイズ変換
    SDL_Point p = window->getWindowSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    // スコア計算
    score += delta * GameConfig::SCORE_RATE;  // 生存時間に重点
    scoreText->setText("Score: " + std::to_string(static_cast<int>(score)));
    // 更新
    player->update(delta, b);
    for(auto& e : enemies) e->update(delta, b);
    // 衝突判定
    for(auto& e : enemies){
        if(GameUtil::intersects(player->getCollisionRect(), e->getCollisionRect())){
            state = GameState::GameOver;
        }
    }
}

/**
 * @brief タイトルをフェードイン＆点滅させる
 * 
 * @param delta 
 */
void Game::updateTitle(double delta){
    /* ----- フェードイン ----- */
    if(titleFade < 1.0){
        // 2秒
        titleFade += delta * 0.5;
        if(titleFade > 1.0){
            titleFade = 1.0;    // 1.0を超えないように
        }
        // 徐々に実体化
        Uint8 alpha = static_cast<Uint8>(titleFade * 255);
        gameTitleText->setAlpha(alpha);
    }

    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer->clear();
    // ゲームの状態で分岐
    switch(state){
    case GameState::Title:
        fpsText->draw(*renderer, 20, 20);
        // 中央にタイトル
        gameTitleText->draw(
            *renderer,
            GameConfig::WINDOW_WIDTH/2 - gameTitleText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/3 - gameTitleText->getHeight()/2
        );

        // 下に「Press ENTER to Start」
        if(blinkVisible){
            titleText->draw(
                *renderer,
                GameConfig::WINDOW_WIDTH/2 - titleText->getWidth()/2,
                GameConfig::WINDOW_HEIGHT/2 - titleText->getHeight()/2
            );
        }
        break;
    case GameState::Playing:
        fpsText->draw(*renderer, 20, 20);
        scoreText->draw(*renderer, 20, 50);
        player->draw(*renderer);
        for(auto& e : enemies) e->draw(*renderer);
        break;
    case GameState::GameOver:
        fpsText->draw(*renderer, 20, 20);
        scoreText->draw(*renderer, 20, 50);
        gameOverText->draw(
            *renderer,
            GameConfig::WINDOW_WIDTH/2 - gameOverText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/2 - gameOverText->getHeight()/2
        );
        break;
    }
   
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

