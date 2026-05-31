#ifndef BACKGROUNDDEFINITION_H
#define BACKGROUNDDEFINITION_H

#include <span>
#include <stdexcept>

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
        static constexpr BackgroundLayerDef Forest[] = {
            {BgLayerTextureId::Sky,         0.0}, 
            {BgLayerTextureId::Mountain,    0.2}, 
            {BgLayerTextureId::Forest,      0.4}, 
        };
        // 暗い森背景の構成
        static constexpr BackgroundLayerDef DarkForest[] = {
            {BgLayerTextureId::DarkSky,         0.0}, 
            {BgLayerTextureId::DarkMountain,    0.2}, 
            {BgLayerTextureId::DarkForest,      0.4}, 
        };
        // 地獄森の背景の構成
        static constexpr BackgroundLayerDef HellForest[] = {
            {BgLayerTextureId::HellSky,         0.0}, 
            {BgLayerTextureId::HellMountain,    0.2}, 
            {BgLayerTextureId::HellForest,      0.4}, 
        };
        // 単色背景
        static constexpr BackgroundLayerDef LightBg[] = {
            {BgLayerTextureId::Sky,         0.0}, 
        };
        static constexpr BackgroundLayerDef DarkBg[] = {
            {BgLayerTextureId::DarkSky,         0.0}, 
        };
        switch(id){
        case BackgroundId::Forest:      return Forest;
        case BackgroundId::DarkForest:  return DarkForest;
        case BackgroundId::HellForest:  return HellForest;
        case BackgroundId::LightBg:     return LightBg;
        case BackgroundId::DarkBg:      return DarkBg;
        }
        // return {};で背景なしでもよいが，バグに気づくようにエラーとする
        throw std::runtime_error("Unknown BackgroundId");
    }
}

#endif  // BACKGROUNDDEFINITION_H
