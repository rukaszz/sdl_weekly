#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

class Texture;
class Renderer;

class Sprite{
private:
    // テクスチャの差し替えを考えたときに，&(参照)では初期化後に変更できない
    // ポインタを持つことで，テクスチャのアドレスの参照を変えることで差し替えを実現する
    Texture* texture;
    SDL_Rect src;   // 元画像の切り出し領域
    SDL_Rect dst;   // 描画サイズ(可変)

    // 1フレーム中の幅と高さ
    int frameWidth;
    int frameHeight;

public:
    // コンストラクタ
    Sprite(Texture& tex, int fw, int fh);
    // setter
    // 描画するテクスチャ矩形の左上の座標セット
    void setPosition(int x, int y);
    // 表示する画像サイズ調整用
    void setDrawSize(int w, int h);
    // スプライトシートの表示領域切り替え用
    void setSrcRect(int x, int y, int w, int h);
    // 横並びのスプライトのフレーム切り替え用
    void setFrame(int frameIndex);
    // フレームのサイズの情報を切替える用
    void setFrameSize(int frame_W, int frame_H);
    // テクスチャをセットする切り替え用関数
    void setTexture(Texture& tex);
    // getter
    // テクスチャ取得用
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