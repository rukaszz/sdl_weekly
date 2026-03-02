#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Texture{
private:
    SDL_Texture* tex;

public:
    // 幅，高さ
    int w, h;

    // コンストラクタ・デストラクタ
    Texture(SDL_Renderer* renderer, const std::string& path);
    ~Texture();

    SDL_Texture* get() const{
        return tex;
    }
};

#endif  // TEXTURE_H