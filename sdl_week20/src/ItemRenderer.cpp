#include "ItemRenderer.hpp"

#include <vector>

#include "Renderer.hpp"
#include "Texture.hpp"
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
void ItemRenderer::render(Renderer& renderer, Camera& camera) const{
    
    for(auto& i : items){

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
        return textures.fireflower;
    }
    // 原則到達しない想定(フォールバックとしてコインを返す)
    return textures.coin;  // #include <utility> std::unreachable();
}
