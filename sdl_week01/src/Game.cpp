#include "Game.hpp"
#include <SDL2/SDL.h>
#include <iostream>

/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game()
    : window("Test", 400, 500), 
      renderer(window.get()), 
      player(100, 100), 
      running(true)
{

}

/**
 * @brief ゲーム実行用関数
 * 
 */
void Game::run(){
    Uint32 timer_end = SDL_GetTicks();
    Uint32 fpsTimer = SDL_GetTicks();
    int fpsCounter = 0;

    while(running){
        // 時間の計測
        Uint32 timer_start = SDL_GetTicks();
        double delta = (timer_start - timer_end) / 1000.0;
        timer_end = timer_start;
        // 各種処理の実施
        processEvents();
        update(delta);
        render();
        // フレームレートの計算とfps計測
        ++fpsCounter;
        if(timer_start - fpsTimer >= 1000){
            std::cout << "FPS: " << fpsCounter << std::endl;
            fpsCounter = 0;
            fpsTimer = timer_start;
        }
        // fpsキャップ(最大60fps)
        fpsFrameRate(timer_start);
    }
}

/**
 * @brief fpsキャップを実装(fpsを固定化するため)
 * 
 * @param fps_timer_start: 計測点
 */
void Game::fpsFrameRate(Uint32 fps_timer_start){
    Uint32 frame_duration = SDL_GetTicks() - fps_timer_start;
    
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
    player.update(delta, keystate, renderer.getOutputSize());
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer.clear();
    renderer.drawRect(player.getRect(), SDL_Color{255, 255, 255, 255});
    renderer.present();
}