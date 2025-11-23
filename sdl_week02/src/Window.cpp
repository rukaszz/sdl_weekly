#include "Window.hpp"
#include <SDL2/SDL_image.h>
#include <stdexcept>

/*
 * Windowクラスは画面表示の基礎部分
 * あくまでもウィンドウの表示に関わる部分※描画はまた別
 */

/** 
 * @brief Construct a new Window:: Window object
 * 
 * @param title: タイトルを記述(SDL_CreateWindowはchar*なのでc_str()が必須)
 * @param width: windowの幅
 * @param height: windowの高さ
 */

Window::Window(const std::string& title, int width, int height){
    // 初期化
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        throw std::runtime_error("SDL_Init failed. ");
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        throw std::runtime_error("IMG_Init failed. ");
    }
    // windowオブジェクト生成
    window = SDL_CreateWindow(
        title.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        SDL_WINDOW_SHOWN
    );
    // 生成がうまくいかなかったら終了
    if(!window){
        SDL_Quit();
        throw std::runtime_error("SDL_CreateWindow failed. ");
    }
}

/**
 * @brief Destroy the Window:: Window object
 * 
 */
Window::~Window(){
    // Windowオブジェクト破棄
    SDL_DestroyWindow(window);
    SDL_Quit();
}
