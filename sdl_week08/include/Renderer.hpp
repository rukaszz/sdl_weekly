#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>

class Sprite;
struct Camera;

class Renderer{
private:
    // SDL_Rendererオブジェクト
    SDL_Renderer* renderer;
    // デバッグ用
    void drawTexture(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst);
    void drawTextureEx(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip);

public:
    // コンストラクタ・デストラクタ
    Renderer(SDL_Window* window);
    ~Renderer();

    // 描画処理関数
    void clear();
    void present();
    void draw(const Sprite& s, bool flipX);
    void draw(const Sprite& s, Camera& camera, bool flipX);
    SDL_Point getOutputSize() const;
    void drawText(SDL_Texture* tex, int x, int y, int w, int h);
    // 床ブロックテスト用
    void drawRect(const SDL_Rect& rect, SDL_Color color);

    // rendererオブジェクト返却用
    SDL_Renderer* get() const{
        return renderer;
    }
};

#endif  // RENDERER_H