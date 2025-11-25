#include "Sprite.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param texture 
 */
Sprite::Sprite(Texture& tex)
    : texture(tex)
{
    src = {0, 0, tex.w, tex.h};
    dst = {0, 0, tex.w, tex.h};
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
 * @brief テクスチャのデータをレンダラーへ描画させる
 * 
 * @param renderer: レンダラークラスのオブジェクト 
 */
void Sprite::draw(Renderer& renderer){
    renderer.drawTexture(texture.get(), &src, &dst);
}
