#include "Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

/**
 * @brief 最小限の処理を記述するmain関数
 * ここはGameを呼ぶだけ
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[]){
    Game game;
    game.run();
    return 0;

    /*
    // Gameで管理しているオブジェクトを破棄させるスコープ
    {
        Game game;
        game.run();
    }
    // Gameが破棄されたらSDLを終了
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
    */
}