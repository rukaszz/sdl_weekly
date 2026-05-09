#ifndef BACKGROUNDDEFINITION_H
#define BACKGROUNDDEFINITION_H

#include <span>
#include "BackgroundId.hpp"

/**
 * @brief 背景の構造を定義するヘッダファイル 
 * 定義は最背面から手前の順で定義されている
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
    /*
    inline std::vector<BackgroundLayerDef> getLayerDefs(BackgroundId id){
        switch (id){
        case BackgroundId::Forest:
            // 森背景の構成
            return{
                {BgLayerTextureId::Sky,         0.0}, 
                {BgLayerTextureId::Mountain,    0.2}, 
                {BgLayerTextureId::Forest,      0.4}, 
            };
        case BackgroundId::DarkForest:
            // 暗い森背景の構成
            return{
                {BgLayerTextureId::DarkSky,         0.0}, 
                {BgLayerTextureId::DarkMountain,    0.2}, 
                {BgLayerTextureId::DarkForest,      0.4}, 
            };
        case BackgroundId::HellForest:
            // 地獄森の背景の構成
            return{
                {BgLayerTextureId::HellSky,         0.0}, 
                {BgLayerTextureId::HellMountain,    0.2}, 
                {BgLayerTextureId::HellForest,      0.4}, 
            };
        default:
            return{};
        }
    }
    */
    
    /**
     * @brief Get the Layer Defs object
     * 与えられた背景IDに応じた背景の構成を返却するヘルパ関数
     * vectorを返す方式は呼び出されるたびにヒープの確保が走るのでコストが高い
     * →spanを用いることで，配列のコピーが発生せず低コストで配列を扱える
     * また，static constexprでコンパイル時に計算させている
     * 
     * @param id 
     * @return std::span<const BackgroundLayerDef> 
     */
    inline std::span<const BackgroundLayerDef> getLayerDefs(BackgroundId id){
        // 森背景の構成
        static constexpr BackgroundLayerDef forest[] = {
            {BgLayerTextureId::Sky,         0.0}, 
            {BgLayerTextureId::Mountain,    0.2}, 
            {BgLayerTextureId::Forest,      0.4}, 
        };
        // 暗い森背景の構成
        static constexpr BackgroundLayerDef darkForest[] = {
            {BgLayerTextureId::DarkSky,         0.0}, 
            {BgLayerTextureId::DarkMountain,    0.2}, 
            {BgLayerTextureId::DarkForest,      0.4}, 
        };
        // 地獄森の背景の構成
        static constexpr BackgroundLayerDef hellForest[] = {
            {BgLayerTextureId::HellSky,         0.0}, 
            {BgLayerTextureId::HellMountain,    0.2}, 
            {BgLayerTextureId::HellForest,      0.4}, 
        };
        switch(id){
        case BackgroundId::Forest:      return forest;
        case BackgroundId::DarkForest:  return darkForest;
        case BackgroundId::HellForest:  return hellForest;
        }
        
        return{};
    }
}

#endif  // BACKGROUNDDEFINITION_H
