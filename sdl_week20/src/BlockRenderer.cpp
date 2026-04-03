#include "BlockRenderer.hpp"

#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Block.hpp"
#include "GameEvent.hpp"

/**
 * @brief Construct a new Block Renderer:: Block Renderer object
 * 
 * @param blockRectCache_ 
 * @param textures_ 
 */
BlockRenderer::BlockRenderer(
    const std::vector<Block>& blocks_, 
    const std::vector<SDL_Rect>& blockRectCaches_, 
    const BlockTextureContext& textures_
) : blocks(blocks_)
  , blockRects(blockRectCaches_)
  , textures(textures_)
{

}

/**
 * @brief ブロックのレンダリング
 * 
 * @param renderer 
 * @param camera 
 */
void BlockRenderer::render(Renderer& renderer, const Camera& camera) const{
    // blockを走査
    for(std::size_t i = 0; i < blocks.size(); ++i){
        // blockを取得
        const Block& block = blocks[i];
        // Emptyは描画しない
        if(block.type == BlockType::Empty){
            continue;
        }
        // 対応するブロック矩形，テクスチャを取得
        const SDL_Rect br = blockRects[i];
        const Texture& blockTex = selectTexture(block.type);
        // 描画
        renderer.drawImage(blockTex, br, camera);
    }
}

/**
 * @brief 与えられたブロックタイプに対応するテクスチャを返す
 * 
 * @param type 
 * @return const Texture& 
 */
const Texture& BlockRenderer::selectTexture(BlockType type) const{
    switch (type){
    case BlockType::Standable:   
        return textures.standable;
    case BlockType::Question:    
        return textures.question;
    case BlockType::UsedQuestion:
        return textures.usedQuestion;
    case BlockType::Breakable:   
        return textures.breakable;
    case BlockType::DropThrough: 
        return textures.dropThrough;
    case BlockType::Damage:      
        return textures.damage;
    case BlockType::Clear:       
        return textures.clear;
    case BlockType::Empty:       
        break;
    }
    // 原則到達しない想定(フォールバックとしてstandableを返す)
    return textures.standable;  // #include <utility> std::unreachable();
}
