#ifndef SDLSYSTEM_H
#define SDLSYSTEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <stdexcept>

/**
 * @brief SDL関係のサブシステムを管理するシステム
 * 現状，初期化処理が失敗した場合すでに初期化済みのシステムが残ってしまう
 * →structでネストしてコンストラクタとデストラクタを構築することで対応可能
 * 
 */
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
        // SDL_TTF初期化
        if((TTF_Init() != 0)){
            throw std::runtime_error(TTF_GetError());
        }
        // SDL_Mixer初期化
        // 44100Hz, ステレオ, バッファ2048サンプル
        int mixFlags = MIX_INIT_OGG;
        if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
            throw std::runtime_error(Mix_GetError());
        }
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
            throw std::runtime_error(Mix_GetError());
        }
    }
    // 初期化順序の逆(RAII)
    ~SdlSystem(){
        Mix_CloseAudio();   // Mixerデバイスの終了
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    // 二重解放防止用のコンストラクタ宣言
    SdlSystem(const SdlSystem&) = delete;
    SdlSystem& operator=(const SdlSystem&) = delete;
};

#endif  // SDLSYSTEM_H
