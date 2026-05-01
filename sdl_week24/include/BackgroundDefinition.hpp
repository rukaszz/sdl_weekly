#ifndef BACKGROUNDDEFINITION_H
#define BACKGROUNDDEFINITION_H

#include <vector>
#include "BackgroundId.hpp"

/**
 * @brief 背景の構造を定義するヘッダファイル 
 * 
 */

/**
 * @brief 背景のレイヤの定義
 * どの順番で描画するなどを定義している
 * 
 */
struct BackgroundLayerDef{
    BgLayerTextureId texId;
    double parallaxFactor;
};

namespace BackgroundDefinition{
    /**
     * @brief Get the Layerdef object
     * 与えられた背景IDに応じた背景の構成を返却するヘルパ関数
     * 
     * @param id 
     * @return std::vector<BackgroundLayerDef> 
     */
    inline std::vector<BackgroundLayerDef> getLayerDefs(BackgroundId id){
        switch (id){
        case BackgroundId::Forest:
            // 森背景の構成
            return{
                {BgLayerTextureId::Sky,         0.0}, 
                {BgLayerTextureId::Mountain,    0.0}, 
                {BgLayerTextureId::Forest,      0.0}, 
            };
        case BackgroundId::DarkForest:
            // 暗い森背景の構成
            return{
                {BgLayerTextureId::DarkSky,         0.0}, 
                {BgLayerTextureId::DarkMountain,    0.0}, 
                {BgLayerTextureId::DarkForest,      0.0}, 
            };
        case BackgroundId::HellForest:
            // 地獄森の背景の構成
            return{
                {BgLayerTextureId::HellSky,         0.0}, 
                {BgLayerTextureId::HellMountain,    0.0}, 
                {BgLayerTextureId::HellForest,      0.0}, 
            };
        default:
            return{};
        }
    }
}

#endif  // BACKGROUNDDEFINITION_H
