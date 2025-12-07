#include "Window.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <string>
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
        throw std::runtime_error(std::string("SDL_CreateWindow failed. ") + SDL_GetError());
    }
}

/**
 * @brief Destroy the Window:: Window object
 * 
 */
Window::~Window(){
    // Windowオブジェクト破棄
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

/**
 * @brief ウィンドウのサイズ(描画範囲)を返却する
 * 
 * @return SDL_Point 
 */
SDL_Point Window::getWindowSize() const{
    SDL_Point p{0, 0};
    // windowオブジェクトの存在チェック
    if(!window){
        return p;
    }
    SDL_GetWindowSize(window, &p.x, &p.y);
    return p;
}
