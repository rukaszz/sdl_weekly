// 定数
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>
#include <vector>


/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game(){
    window = std::make_unique<Window>("Test", 800, 800);
    renderer = std::make_unique<Renderer>(window->get());
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    player = std::make_unique<Player>(*playerTexture);
    
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    auto enemyTex = enemyTexture.get();
    auto e1 = std::make_unique<Enemy>(*enemyTex);
    auto e2 = std::make_unique<Enemy>(*enemyTex);
    e1->setPosition(300, 50);
    e2->setPosition(400, 250);
    enemies.push_back(std::move(e1));
    enemies.push_back(std::move(e2));
    
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
        std::cout << "GameOver!!" << std::endl;
    }
    renderer->present();
}


void Game::reset(){
    player->setPosition(100, 250);
    enemies[0]->setPosition(50, 50);
    enemies[1]->setPosition(250, 650);
}
