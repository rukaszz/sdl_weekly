#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>

class Texture;
class Sprite;
struct Camera;

class Renderer{
private:
    // SDL_Rendererオブジェクト
    SDL_Renderer* renderer;
    // テクスチャ描画用
    void drawTexture(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst);
    void drawTextureEx(SDL_Texture* tex, const SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip);

public:
    // コンストラクタ・デストラクタ
    Renderer(SDL_Window* window);
    ~Renderer();

    // 描画処理関数
    void clear();
    void present();
    // スプライト描画関数
    void draw(const Sprite& s, bool flipX);
    void draw(const Sprite& s, Camera& camera, bool flipX);
    // 描画可能範囲の取得
    SDL_Point getOutputSize() const;
    // テキスト描画用
    void drawText(SDL_Texture* tex, int x, int y, int w, int h);
    // 床ブロック(テスト)用
    void drawRect(const SDL_Rect& rect, SDL_Color color);
    void drawRect(const SDL_Rect& rect, SDL_Color color, const Camera& camera);   // カメラ考慮版
    // drawTextureのpublicメソッド：スプライトシート用
    void drawSprite(const Texture& tex, const SDL_Rect& src, SDL_Rect dst);
    void drawSprite(const Texture& tex, const SDL_Rect& src, SDL_Rect dst, const Camera& camera); // カメラ考慮版
    // drawTextureのpublicメソッド：静止画像用
    void drawImage(const Texture& tex, SDL_Rect dst);
    void drawImage(const Texture& tex, SDL_Rect dst, const Camera& camera); // カメラ考慮版

    // getter/setter
    // rendererオブジェクト返却用
    SDL_Renderer* get() const{
        return renderer;
    }
};

#endif  // RENDERER_H