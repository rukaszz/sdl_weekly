#include "Sprite.hpp"
#include "Texture.hpp"

#include <SDL2/SDL.h>

#include <cassert>

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param texture 
 */
Sprite::Sprite(Texture& tex, int fw, int fh)
        : texture(&tex) // ポインタなので参照
        , frameWidth(fw)
    , frameHeight(fh)
{
    // スプライトシート差し替え時の事故防止
    assert(fw <= tex.w);
    assert(fh <= tex.h);
    src = {0, 0, fw, fh};
    dst = {0, 0, fw, fh};
}

/**
 * @brief Set the Position object
 * レンダラー上のテクスチャコピー先を示す矩形の座標をセットする
 * 
 * @param x 
 * @param y
 */
void Sprite::setPosition(int x, int y){
    dst.x = x;
    dst.y = y;
}

/**
 * @brief 表示する画像のサイズ調整用関数
 * 
 * @param w 
 * @param h 
 */
void Sprite::setDrawSize(int w, int h){
    dst.w = w;
    dst.h = h;
}

/**
 * @brief スプライトシートの表示領域切り替え用の入口
 * 
 * 任意のスプライト領域を手動で設定する．
 * setFrame()で毎フレームsrcを上書きするため，アニメーション中の使用は禁止
 * 
 * @param x 
 * @param y 
 * @param w 
 * @param h 
 */
void Sprite::setSrcRect(int x, int y, int w, int h){
    src = {x, y, w, h};
}

/**
 * @brief スプライトが横並びであることを前提として，フレームを切り替える
 * 
 * @param frameIndex: frameWidthに従って1, 2, ...とずれていくための添字
 */
void Sprite::setFrame(int frameIndex){
    src.x = frameIndex * frameWidth;
    // src.y = 0;
    // src.w = frameWidth;
    // src.h = frameHeight;
}

/**
 * @brief 参照するスプライトシートのテクスチャが変わった際に情報を更新する関数
 * 
 * @param frame_W 
 * @param frame_H 
 */
void Sprite::setFrameSize(int frame_W, int frame_H){
    src.w = frame_W;
    src.h = frame_H;
    dst.w = frame_W;
    dst.h = frame_H;
}

/**
 * @brief テクスチャの切り替え用setter
 *  差し替えたいテクスチャの参照(tex)のアドレスをポインタへ渡して差し替える
 * 
 * @param tex 
 */
void Sprite::setTexture(Texture& tex){
    texture = &tex;
}

/**
 * @brief Get the Texture object
 * Sprite.hppでTextureクラスを知ってはいけないので分離
 * 
 * @return SDL_Texture* 
 */
SDL_Texture* Sprite::getTexture() const{
    return texture->get();
}
