#include "Sprite.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param texture 
 */
Sprite::Sprite(Texture& tex, int fw, int fh)
    : texture(tex)
    , frameWidth(fw)
    , frameHeight(fh)
{
    src = {0, 0, fw, fh};
    dst = {0, 0, fw, fh};
}

/**
 * @brief Set the Position object
 * テクスチャへのレンダラー上のコピー先の矩形の座標をセットする
 * 
 * @param x 
 * @param y 
 */
void Sprite::setPosition(int x, int y){
    dst.x = x;
    dst.y = y;
}

/**
 * @brief 表示する画像のサイズ調整用関数
 * 
 * @param w 
 * @param h 
 */
void Sprite::setSize(int w, int h){
    dst.w = w;
    dst.h = h;
}

/**
 * @brief スプライトシートの表示領域切り替え用の入口
 * 
 * @param x 
 * @param y 
 * @param w 
 * @param h 
 */
void Sprite::setSrcRect(int x, int y, int w, int h){
    src = {x, y, w, h};
}


void Sprite::setFrame(int frameIndex){
    src.x = frameIndex * frameWidth;
    src.y = 0;
    src.w = frameWidth;
    src.h = frameHeight;
}

/**
 * @brief テクスチャのデータをレンダラーへ描画させる
 * 
 * @param renderer: レンダラークラスのオブジェクト 
 */
void Sprite::draw(Renderer& renderer){
    renderer.drawTexture(texture.get(), &src, &dst);
}
