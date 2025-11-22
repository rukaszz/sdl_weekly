#include "Renderer.hpp"
#include <stdexcept>

/**
 * Renderer: SDL_Renderer の RAII ラッパ。
 * 描画処理（クリア、描画、Present）を Window から切り離し、
 * SDL に依存する処理をこのクラスに集約する。
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

/**
 * @brief 描画可能範囲を返す関数
 * windowでこの機能を実現するなら，SDL_GL_GetDrawableSize()を使う
 * 現システムはrendererを使っているのでRendererに責務をもたせる
 * 
 * @return SDL_Point: x, y(double)
 */
SDL_Point Renderer::getOutputSize() const{
    SDL_Point p{0, 0};
    // rendererオブジェクトの存在チェック
    if(!renderer){
        return p;
    }
    SDL_GetRendererOutputSize(renderer, &p.x, &p.y);
    return p;
}