#include "BackgroundPresetBuilder.hpp"

#include "BackgroundRenderer.hpp"
#include "BackgroundDefinition.hpp"
#include "BackgroundId.hpp"

#include "Texture.hpp"
#include "RenderAssetContext.hpp"
#include "StageDefinition.hpp"

// テクスチャ選択ヘルパ関数は外部に公開しない
namespace{
    /**
     * @brief テクスチャデータコンテキストと背景IDを紐付けるヘルパ関数
     * 
     * @param btc 
     * @param texId 
     * @return const Texture& 
     */
    const Texture& selectLayerTexture(
        const BackgroundTextureContext& btc, 
        BgLayerTextureId texId
    )
    {
        switch(texId){
        case BgLayerTextureId::Sky:             return btc.sky;
        case BgLayerTextureId::Mountain:        return btc.mountain;
        case BgLayerTextureId::Forest:          return btc.forest;
        case BgLayerTextureId::DarkSky:         return btc.darkSky;
        case BgLayerTextureId::DarkMountain:    return btc.darkMountain;
        case BgLayerTextureId::DarkForest:      return btc.darkForest;
        case BgLayerTextureId::HellSky:         return btc.hellSky;
        case BgLayerTextureId::HellMountain:    return btc.hellMountain;
        case BgLayerTextureId::HellForest:      return btc.hellForest;
        }
    }
    /**
     * @brief テクスチャデータコンテキストと装飾タイプを紐付けるヘルパ関数
     * 
     * @param btc 
     * @param type 
     * @return const Texture& 
     */
    const Texture& selectDecorationTexture(
        const BackgroundTextureContext& btc, 
        BgDecorationType type
    )
    {
        switch(type){
        case BgDecorationType::Cloud:   return btc.cloudTexture;
        case BgDecorationType::Star:    return btc.starTexture;
        case BgDecorationType::DarkSun: return btc.darkSunTexture;
        }
    }
}

/**
 * @brief BackgroundRendererのclear〜addLayer/addDecorationまでを一括で行う 
 * 
 * @param renderer 
 * @param textures 
 * @param stageDef 
 */
void BackgroundPresetBuilder::build(
    BackgroundRenderer& bgRenderer,
    const BackgroundTextureContext& textures,
    const StageDefinition& stageDef
)
{   
    // 背景・装飾のクリア
    bgRenderer.clearLayers();
    bgRenderer.clearDecoration();
    // 背景の構成
    for(const auto& bgLayerDef : BackgroundDefinition::getLayerDefs(stageDef.backgroundId)){
        bgRenderer.addLayer(
            selectLayerTexture(textures, bgLayerDef.texId), 
            bgLayerDef.parallaxFactor
        );
    }
    // 装飾の構成
    for(const auto& bgDecoDef : stageDef.bgDecorations){
        bgRenderer.addDecoration(
            selectDecorationTexture(textures, bgDecoDef.type),
            bgDecoDef.world_X, bgDecoDef.screen_Y, bgDecoDef.parallaxFactor 
        );
    }
}
