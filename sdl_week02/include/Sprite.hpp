#ifndef SPRITE_H
#define SPRITE_H

#include "Texture.hpp"
#include <SDL2/SDL.h>

class Renderer;

class Sprite{
private:
    Texture& texture;
    SDL_Rect src;   // 元画像の切り出し領域
    SDL_Rect dst;   // 描画サイズ(可変)

    // 1フレーム中の幅と高さ
    int frameWidth;
    int frameHeight;

public:
    Sprite(Texture& tex, int fw, int fh);

    void setPosition(int x, int y);
    void setDrawSize(int w, int h);
    void setSrcRect(int x, int y, int w, int h);
    void setFrame(int frameIndex);

    // src, dstのgetter
    const SDL_Rect& getSrcRect() const{
        return src;
    }
    const SDL_Rect& getDstRect() const{
        return dst;
    }
    // Textureのgetter
    SDL_Texture* getTexture() const{
        return texture.get();
    }
    // 幅，高さgetter
    int getWidth() const{
        return dst.w;
    }
    int getHeight() const{
        return dst.h;
    }
    // 1フレーム中の幅と高さgetter
    int getFrameWidth() const{
        return frameWidth;
    }
    int getFrameHeight() const{
        return frameHeight;
    }
};

#endif  // SPRITE_H