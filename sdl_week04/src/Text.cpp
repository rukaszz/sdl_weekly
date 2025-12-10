#include "Text.hpp"
#include "Renderer.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <stdexcept>

/**
 * @brief Construct a new Text:: Text object
 * 
 * @param fontPath: フォントの配置場所(原則assets/font)
 * @param fontSize: フォントサイズ 
 */
Text::Text(const std::string& fontPath, int fontSize){
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if(!font){
        throw std::runtime_error(std::string("TTF_OpenFont failed: ") + TTF_GetError());
    }
}

/**
 * @brief Destroy the Text:: Text object
 * 
 */
Text::~Text(){
    if(font){
        TTF_CloseFont(font);
        font = nullptr;
    }
}

/**
 * @brief 一時的な描画用Textureを返す関数．所有権は呼び出し側が持つ
 * 破棄も呼び出し側が実施する
 * 
 * @param renderer: SDL_Renderer&
 * @param text: 描画する文字 
 * @param outW: 幅 
 * @param outH: 高 
 * @param color: 色(SDL_Color) 
 * @return SDL_Texture* 
 */
SDL_Texture* Text:: renderText(SDL_Renderer* renderer, const std::string& text, int& outW, int& outH, SDL_Color color){
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if(!surf){
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    outW = surf->w;
    outH = surf->h;
    
    SDL_FreeSurface(surf);
    // SDL_DestoryTexture()は呼び出し側で実施する
    return tex;
}

