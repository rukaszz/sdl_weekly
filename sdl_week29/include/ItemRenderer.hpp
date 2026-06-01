#ifndef ITEMRENDERER_H
#define ITEMRENDERER_H

#include <vector>
#include <cstdint>

#include "RenderAssetContext.hpp"

class Renderer;
class Texture;
struct Camera;
class Item;
enum class ItemType : std::uint8_t;

class ItemRenderer{
private:
    // ItemSytemとItemRendererの両方がitemsを参照するので，itemsの参照を持っている
    const std::vector<Item>& items;
    const ItemTextureContext& textures;

public:
    ItemRenderer(
        const std::vector<Item>& items_, 
        const ItemTextureContext& textures_
    );
    void render(Renderer& renderer, const Camera& camera) const;

private:
    const Texture& selectTexture(ItemType type) const;
};

#endif  // ITEMRENDERER_H
