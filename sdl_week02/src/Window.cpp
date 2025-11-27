#include "Window.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
}

SDL_Point Window::getWindowSize() const{
    SDL_Point p{0, 0};
    // windowオブジェクトの存在チェック
    if(!window){
        return p;
    }
    SDL_GetWindowSize(window, &p.x, &p.y);
    return p;
}
