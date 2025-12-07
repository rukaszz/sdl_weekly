#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

class Texture;
class Renderer;

class Sprite{
private:
    Texture& texture;
    // SDL_Texture& sdl_texture;
    SDL_Rect src;   // 元画像の切り出し領域
    SDL_Rect dst;   // 描画サイズ(可変)

    // 1フレーム中の幅と高さ
    int frameWidth;
    int frameHeight;

public:
    Sprite(Texture& tex, int fw, int fh);
    // Sprite(SDL_Texture& tex, int fw, int fh);

    void setPosition(int x, int y);
    void setDrawSize(int w, int h);
    void setSrcRect(int x, int y, int w, int h);
    void setFrame(int frameIndex);

    SDL_Texture* getTexture() const;

    // src, dstのgetter
    const SDL_Rect& getSrcRect() const{
        return src;
    }
    const SDL_Rect& getDstRect() const{
        return dst;
    }
    
    // 描画するときの幅，高さgetter
    int getDrawWidth() const{
        return dst.w;
    }
    int getDrawHeight() const{
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