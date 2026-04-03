#include "ItemRenderer.hpp"

#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Item.hpp"
#include "GameEvent.hpp"

/**
 * @brief Construct a new Item Renderer:: Item Renderer object
 * 
 * @param items_ 
 * @param textures_ 
 */
ItemRenderer::ItemRenderer(
    const std::vector<Item>& items_, 
    const ItemTextureContext& textures_
) : items(items_)
  , textures(textures_)
{

}

/**
 * @brief アイテムのレンダリング
 * 
 * @param renderer 
 * @param camera 
 */
void ItemRenderer::render(Renderer& renderer, const Camera& camera) const{
    // itemsを舐める
    for(const auto& item : items){
        // 非活性は処理しない
        if(!item.isActive()){
            continue;
        }
        // 描画矩形の取得(getCollisionRectは仮※専用のメソッドがあったほうが良い)
        SDL_Rect drawRect = item.getDrawRect();
        // 対応するアイテムのテクスチャを取得
        const Texture& itemTex  = selectTexture(item.getItemType());
        // rendererへ渡す(静止画用メソッドを使用)
        renderer.drawImage(itemTex, drawRect, camera);
    }
}

/**
 * @brief 与えられたアイテムに対応するテクスチャを返す
 * 
 * @param type 
 * @return const Texture& 
 */
const Texture& ItemRenderer::selectTexture(ItemType type) const{
    switch (type){
    case ItemType::Coin:
        return textures.coin;
    case ItemType::Mushroom:
        return textures.mushroom;
    case ItemType::FireFlower:
        return textures.fireFlower;
    }
    // 原則到達しない想定(フォールバックとしてコインを返す)
    return textures.coin;  // #include <utility> std::unreachable();
}
