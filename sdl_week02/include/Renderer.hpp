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
    void drawSprite(const Sprite& sprite);
    SDL_Point getOutputSize() const;

    // rendererオブジェクト返却用
    SDL_Renderer* get() const{
        return renderer;
    }
};

#endif  // RENDERER_H