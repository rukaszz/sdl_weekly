#ifndef BLOCKRENDERER_H
#define BLOCKRENDERER_H

#include <vector>
#include <cstdint>

#include <SDL2/SDL.h>

#include "RenderAssetContext.hpp"

class Renderer;
class Texture;
struct Camera;
struct Block;
enum class BlockType : std::uint8_t;

class BlockRenderer{
private:
    // blockTypeの取得用
    const std::vector<Block>& blocks;
    // 描画はBlockをSDL_Rectに変換したキャッシュblockRectCache
    const std::vector<SDL_Rect>& blockRects;
    const BlockTextureContext& textures;

public:
    BlockRenderer(
        const std::vector<Block>& blocks_, 
        const std::vector<SDL_Rect>& blockRects_,  
        const BlockTextureContext& textures_
    );
    void render(Renderer& renderer, const Camera& camera) const;

private:
    const Texture& selectTexture(BlockType type) const;
};


#endif  // BLOCKRENDERER_H
