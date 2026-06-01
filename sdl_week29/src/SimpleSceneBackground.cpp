#include "SimpleSceneBackground.hpp"

#include "BackgroundPresetBuilder.hpp"
#include "Renderer.hpp"
#include "RenderAssetContext.hpp"

SimpleSceneBackground::SimpleSceneBackground(int w, int h)
    : bgRenderer(w, h)
    , fixedCamera(0.0, 0.0, static_cast<double>(w), static_cast<double>(h))
{

}

/**
 * @brief 背景設定の読み込み
 * BackgroundPresetBuilder::build()のラッパー
 * 
 * @param tex 
 * @param id 
 */
void SimpleSceneBackground::setPreset(
    const BackgroundTextureContext& tex, 
    BackgroundId id
)
{
    BackgroundPresetBuilder::build(bgRenderer, tex, id);
}

void SimpleSceneBackground::render(Renderer& renderer) const{
    bgRenderer.renderBackground(renderer, fixedCamera);
    bgRenderer.renderBgDecoration(renderer, fixedCamera);
}

/**
 * @brief bgRendererクラスのメンバのvectorのクリア
 * 
 */
void SimpleSceneBackground::clear(){
    bgRenderer.clearLayers();
    bgRenderer.clearDecoration();
}
