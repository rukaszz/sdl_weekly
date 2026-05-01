#ifndef BACKGROUNDID_H
#define BACKGROUNDID_H

#include <cstdint>

/**
 * @brief 背景の種類管理用のID
 * 
 */
enum class BackgroundId : std::uint8_t{
    Forest, 
    DarkForest, 
    HellForest, 
};

/**
 * @brief 背景装飾の種類管理用のID
 * 
 */
enum class BgDecorationType : std::uint8_t{
    Cloud, 
    Star, 
    DarkSun, 
};

/**
 * @brief 背景に使用されるテクスチャのID管理用
 * 
 */
enum class BgLayerTextureId : std::uint8_t{
    Sky, 
    Mountain, 
    Forest, 
    DarkSky, 
    DarkMountain, 
    DarkForest, 
    HellSky, 
    HellMountain, 
    HellForest, 
};

#endif  // BACKGROUNDID_H
