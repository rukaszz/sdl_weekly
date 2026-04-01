#ifndef ITEMRENDERER_H
#define ITEMRENDERER_H

#include <vector>

class Renderer;
class Texture;
class Camera;
class Item;
class ItemType;
class ItemTextureContext;

class ItemRenderer{
private:
    // ItemSytemとItemRendererの療法がitemsを参照するので，itemsの参照を持っている
    const std::vector<Item>& items;
    const ItemTextureContext& textures;

public:
    ItemRenderer(
        const std::vector<Item>& items_, 
        const ItemTextureContext& textures_
    );
    void render(Renderer& renderer, Camera& camera) const;

private:
    const Texture& selectTexture(ItemType type) const;
};

#endif  // ITEMRENDERER_H
