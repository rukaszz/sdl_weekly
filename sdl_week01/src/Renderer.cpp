#include "Renderer.hpp"
#include <stdexcept>

/*
* Rendererクラスは描画に関する処理の窓口になるように
*/

/**
 * @brief Construct a new Renderer:: Renderer object
 * 
 * @param window: 描画するwindowオブジェクトを受け取る
 */
Renderer::Renderer(SDL_Window* window){
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        throw std::runtime_error("SDL_CreateRenderer failed. ");
    }
}

/**
 * @brief Destroy the Renderer:: Renderer object
 * 
 */
Renderer::~Renderer(){
    SDL_DestroyRenderer(renderer);
}

/**
 * @brief 対象のオブジェクトを消す関数
 * 描画処理前に初期化するイメージ
 */
void Renderer::clear(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

/**
* レンダラーを提供する関数
 * @brief 
 * 
 */
void Renderer::present(){
    SDL_RenderPresent(renderer);
}

/*
* 矩形描画関数
* SDLの矩形オブジェクトと色を受け取って生成する
* @param rect: SDL_Rectオブジェクト
* @param color: 矩形を塗りつぶす色を指定(List型)※RGB+alpha
*/

/**
 * @brief 矩形描画関数
 * SDLの矩形オブジェクトと色を受け取って生成する
 * 
 * @param rect: SDL_Rectオブジェクトを受け取る 
 * @param color: RGB+alphaをListで受け取る 
 */
void Renderer::drawRect(const SDL_Rect& rect, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}