#include "Renderer.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Text Texture:: Text Texture object
 * 
 * @param r 
 * @param t: Textクラス 
 * @param c 
 */
TextTexture::TextTexture(Renderer& r, Text* t, SDL_Color c)
    : renderer(r)
    , text(t)
    , color(c)
{

}

/**
 * @brief Destroy the Text Texture:: Text Texture object
 * 
 */
TextTexture::~TextTexture(){
    if(tex){
        SDL_DestroyTexture(tex);
    }
}

/**
 * @brief 描画する文字列をセットする
 * テクスチャ生成は文字列が変化した場合のみ実施される
 * 
 * @param s 
 */
void TextTexture::setText(const std::string& s){
    if(s == currentStr){
        return; // 変更がなければtextureを生成しない
    }

    currentStr = s;
    // 既存textureの破棄
    if(tex){
        SDL_DestroyTexture(tex);
    }
    tex = text->renderText(renderer.get(), s, width, height, color);
}

/**
 * @brief レンダラーに描画を依頼する．描画位置を指定可能
 * 
 * @param r 
 * @param x 
 * @param y 
 */
void TextTexture::draw(Renderer& r, int x, int y){
    if(!tex){
        return;
    }
    r.drawText(tex, x, y, width, height);
}

/**
 * @brief テクスチャにアルファチャンネルをセットする
 * 
 * @param alpha 
 */
void TextTexture::setAlpha(Uint8 alpha){
    if(tex){
        // テクスチャに乗算されるアルファ値をセットする
        SDL_SetTextureAlphaMod(tex, alpha);
    }
}

