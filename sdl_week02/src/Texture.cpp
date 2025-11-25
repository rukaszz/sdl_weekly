#include "Texture.hpp"

#include <SDL2/SDL.h>
#include <string>
#include <stdexcept>

/**
 * @brief Construct a new Texture:: Texture object
 * 
 * @param renderer 
 * @param path: 画像のパス 
 */
Texture::Texture(SDL_Renderer* renderer, const std::string& path){
    // Surface: RAMに保持されピクセルデータを読み書きする構造体
    SDL_Surface* surf = IMG_Load(path.c_str());
    if(!surf){
        throw std::runtime_error(std::string("IMG_Load failed. ") + path + " | " + IMG_GetError());
    }
    // エラー処理
    tex = SDL_CreateTextureFromSurface(renderer, surf);
    if(!tex){
        SDL_FreeSurface(surf);
        throw std::runtime_error(std::string("SDL_CreateTextureFromSurface failed. ") + SDL_GetError());
    }
    w = surf->w;
    h = surf->h;
    SDL_FreeSurface(surf);
}

/**
 * @brief Destroy the Texture:: Texture object
 * 
 */
Texture::~Texture(){
    SDL_DestroyTexture(tex);
}
