#include "Renderer.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>
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
 * @brief レンダラーに描画元のテクスチャを引数の位置へ貼り付ける
 * srcとdstのサイズが異なっている場合調整される．
 * 
 * @param tex: 描画元のテクスチャ
 * @param src: テクスチャ内のコピー基の矩形(NULLならテクスチャ全体を使う)
 * @param dst: レンダラー上のコピー先の矩形(NULLならターゲット全体へ引き伸ばされる)
 */
void Renderer::drawTexture(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst){
    SDL_RenderCopy(renderer, tex, src, dst);
}

/**
 * @brief SDL_RenderCopyExのラッパ(テクスチャ版)
 * 
 * SDL_RenderCopyExの引数:
 * SDL_Renderer*: レンダラーのアドレス
 * SDL_Texture*: コピー元テクスチャのアドレス
 * SDL_Rect* src(const): コピー元SDL_Rect(NULLなら全域) 
 * SDL_Rect* dst(const): コピー先SDL_Rect(NULLなら全域)．この領域に合わせる 
 * double angle: dstをコピーするときの画像の角度(半時計回り・度数法)
 * SDL_Point*: dstをコピーするときの画像の回転の中心(NULLならdst.w/2とdsy.h/2)
 * SDL_RendererFlip: テクスチャの上下左右反転を表す
 * 
 * @param tex: コピー元テクスチャ 
 * @param src: コピー元SDL_Rect(NULLなら全域) 
 * @param dst: コピー先SDL_Rect(NULLなら全域)．この領域に合わせる 
 * @param flip: テクスチャの上下左右反転を表す
 */
void Renderer::drawTextureEx(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip){
    SDL_RenderCopyEx(
        renderer, 
        tex, 
        src, 
        dst, 
        0.0, 
        nullptr, 
        flip
    );
}

/**
 * @brief 描画処理を担う反転可能な描画関数
 * SDL_RenderCopyExを呼び出す
 * 
 * SDL_RenderCopyExの引数:
 * SDL_Renderer*: レンダラーのアドレス
 * SDL_Texture*: コピー元テクスチャのアドレス
 * SDL_Rect* src(const): コピー元SDL_Rect(NULLなら全域) 
 * SDL_Rect* dst(const): コピー先SDL_Rect(NULLなら全域)．この領域に合わせる 
 * double angle: dstをコピーするときの画像の角度(半時計回り・度数法)
 * SDL_Point*: dstをコピーするときの画像の回転の中心(NULLならdst.w/2とdsy.h/2)
 * SDL_RendererFlip: テクスチャの上下左右反転を表す
 * 
 * @param sprite 
 * @param flipX: dir == Direction::Leftのように渡ってくる真理値  
 */
void Renderer::draw(const Sprite& sprite, bool flipX){
    SDL_RendererFlip flip = flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    const SDL_Rect& src = sprite.getSrcRect();
    const SDL_Rect& dst = sprite.getDstRect();
    
    SDL_RenderCopyEx(
        renderer, 
        sprite.getTexture(), 
        &src, 
        &dst, 
        0.0, 
        nullptr, 
        flip
    );
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

/**
 * @brief Text描画用処理．Renderer側でTextを描画する専用のAPIを用意する
 * 
 * @param tex: SDL_Texture* 
 * @param x 
 * @param y 
 * @param w 
 * @param h 
 */
void Renderer::drawText(SDL_Texture* tex, int x, int y, int w, int h){
    SDL_Rect dst{x, y, w, h};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
}
