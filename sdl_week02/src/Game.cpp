#include "Window.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game()
    : window("Test", 400, 500), 
      renderer(window.get()), 
      running(true)
{
    // Texture読み込み
    playerTexture = new Texture(renderer.get(), "assets/rhb.png");
    // playerの作成
    player = new Player(*playerTexture);
}

Game::~Game(){
    delete player;
    delete playerTexture;
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
void Game::capFrameRate(Uint32 fps_nowTime){
    Uint32 frame_duration = SDL_GetTicks() - fps_nowTime;
    
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
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    player->update(delta, keystate, renderer.getOutputSize());
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer.clear();
    player->draw(renderer);
    renderer.present();
}