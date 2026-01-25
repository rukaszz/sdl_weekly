#ifndef SDLSYSTEM_H
#define SDLSYSTEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdexcept>

class SdlSystem{
public:
    SdlSystem(){
        // SDL初期化
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
            throw std::runtime_error(SDL_GetError());
        }
        // SDL_IMG初期化
        if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
            throw std::runtime_error(IMG_GetError());
        }
        if((TTF_Init() != 0)){
            throw std::runtime_error(TTF_GetError());
        }
    }
    ~SdlSystem(){
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    SdlSystem(const SdlSystem&) = delete;
    SdlSystem& operator=(const SdlSystem&) = delete;
};

#endif  // SDLSYSTEM_H
