#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>

class Sprite;

class Renderer{
private:
    // SDL_Rendererオブジェクト
    SDL_Renderer* renderer;

public:
    // コンストラクタ・デストラクタ
    Renderer(SDL_Window* window);
    ~Renderer();

    // 描画処理関数
    void clear();
    void present();
    void drawTexture(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst);
    void drawTextureEx(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip);
    void drawSprite(const Sprite& sprite);
    void drawSpriteEx(const Sprite& sprite, SDL_RendererFlip flip);
    void drawSpriteFlip(const Sprite& s, bool flipX);
    SDL_Point getOutputSize() const;

    // rendererオブジェクト返却用
    SDL_Renderer* get() const{
        return renderer;
    }
};

#endif  // RENDERER_H