#include "Game.hpp"
#include <SDL2/SDL.h>

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
    Uint32 last = SDL_GetTicks();

    while(running){
        processEvents();

        Uint32 now = SDL_GetTicks();
        double delta = (now - last) / 1000.0;
        last= now;

        update(delta);
        render();
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
 * @param delta: 差分
 */
void Game::update(double delta){
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    player.update(delta, keystate);
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