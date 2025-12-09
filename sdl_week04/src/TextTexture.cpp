#include "Renderer.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"

#include <SDL2/SDL.h>

TextTexture::TextTexture(SDL_Renderer* r, Text* t, SDL_Color c)
    : renderer(r)
    , text(t)
    , color(c)
{

}

TextTexture::~TextTexture(){
    if(tex){
        SDL_DestroyTexture(tex);
    }
}

void TextTexture::setText(const std::string& s){
    if(s == currentStr){
        return; // 変更がなければtextureを生成しない
    }

    currentStr = s;
    // 既存textureの破棄
    if(tex){
        SDL_DestroyTexture(tex);
    }
    tex = text->renderText(renderer, s, width, height, color);
}

void TextTexture::draw(Renderer& r, int x, int y){
    if(!tex){
        return;
    }
    r.drawText(tex, x, y, width, height);
}
