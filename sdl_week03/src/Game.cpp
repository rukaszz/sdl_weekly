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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>

/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game(){
    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    renderer = std::make_unique<Renderer>(window->get());
    
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    player = std::make_unique<Player>(*playerTexture);
    player->setPosition(GameConfig::PLAYER_START_X, GameConfig::PLAYER_START_Y);
    
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    auto enemyTex = enemyTexture.get();
    for(int i = 0;i < 5; ++i){
        enemies.push_back(std::make_unique<Enemy>(*enemyTex));
    }
    // auto e1 = std::make_unique<Enemy>(*enemyTex);
    // auto e2 = std::make_unique<Enemy>(*enemyTex);
    // e1->setPosition(GameConfig::ENEMY1_START_X, GameConfig::ENEMY1_START_Y);
    // e2->setPosition(GameConfig::ENEMY2_START_X, GameConfig::ENEMY2_START_Y);
    // enemies.push_back(std::move(e1));
    // enemies.push_back(std::move(e2));

    text = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    
    running = true;
}

Game::~Game(){
    /*
     * Renderer→Window→SDLの順で破棄
     * ここでIMG_Quit();, SDL_Quit();
     * をするとSDL内部でRenderer/Textureが破棄され
     * スマートポインタがSDL_DestroyTextureを呼んで二重破棄となるので
     * Window側で破棄する
     * ->week03でリファクタリングする
     * DBusのリークはSDL_Linux側の問題なので無視
     */
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
            std::cout << "FPS: " << fpsCounter << std::endl;
            fpsCounter = 0;
            fpsTimer = nowTime;
            // 1フレーム生存=1点
            if(state == GameState::Playing){
                ++score;
            } 
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
    if(state == GameState::GameOver){
        const Uint8* k = SDL_GetKeyboardState(NULL);
        if(k[SDL_SCANCODE_RETURN]){
            reset();    // PlayerとEnemyを元の位置へ戻す
            state = GameState::Playing;
            score = 0;
        }
        return;
    }
    /* ----- Playing状態 ----- */
    // ウィンドウサイズ変換
    SDL_Point p = window->getWindowSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    // 更新
    player->update(delta, b);
    for(auto& e : enemies) e->update(delta, b);
    
    // 衝突判定
    for(auto& e : enemies){
        if(GameUtil::intersects(player->getSprite(), e->getSprite())){
            std::cout << "Collision!!" << std::endl;
            state = GameState::GameOver;
        }
    }
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer->clear();
    // ゲームの状態で分岐
    if(state == GameState::Playing){
        player->draw(*renderer);
        for(auto& e : enemies) e->draw(*renderer);    
    } else {
        // GameOverの表示
        // std::cout << "GameOver!!" << std::endl;
        displayText("GameOver! Score: " + std::to_string(score), "white");
    }
    renderer->present();
}

/**
 * @brief ゲームの状態をリセットする関数
 * 現状はPlayer/Enemyの位置を初期位置にセットするのみ
 * 
 */
void Game::reset(){
    player->setPosition(GameConfig::PLAYER_START_X, GameConfig::PLAYER_START_Y);
    enemies[0]->setPosition(GameConfig::ENEMY1_START_X, GameConfig::ENEMY1_START_Y);
    enemies[1]->setPosition(GameConfig::ENEMY2_START_X, GameConfig::ENEMY2_START_Y);
}

/**
 * @brief 画面に文字を表示する関数
 * 文字の出現位置は現状画面中央．
 * 
 * @param dispStr: 表示する文字列
 * @param color: 文字列の色．処理内のマッピングのどれかを指定する． 
 */
void Game::displayText(const std::string& dispStr, const std::string& color){
    // 表示する文字の幅と高さ
    int w, h;
    // 色のマッピング
    static const std::unordered_map<std::string, SDL_Color> colorMap = {
        {"white", {255, 255, 255, 255}}, 
        {"black", {0, 0, 0, 255}}, 
        {"red", {255, 0, 0, 255}}, 
        {"green", {0, 255, 0, 255}}, 
        {"blue", {0, 0, 255, 255}}, 
    };
    // 引数から色を調べるイテレータ
    auto it = colorMap.find(color);
    // TextのRenderText呼び出し
    if(it != colorMap.end()){
        SDL_Texture* tex = text->renderText(renderer->get(), dispStr, w, h, it->second);
        renderer->drawText(tex, GameConfig::WINDOW_WIDTH/2 - w/2, GameConfig::WINDOW_HEIGHT/2 - h/2, w, h);
        if(tex){
            SDL_DestroyTexture(tex);
        }
    }
}
