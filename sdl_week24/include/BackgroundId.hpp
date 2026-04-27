#ifndef BACKGROUNDID_H
#define BACKGROUNDID_H

#include <cstdint>

/**
 * @brief 背景画像の種類管理用のID
 * 
 */
enum class BackgroundId : std::uint8_t{
    Forest, 
    DarkForest, 
    HellForest, 
};

/**
 * @brief 背景装飾種類管理用
 * 
 */
enum class BgDecorationType : std::uint8_t{
    Cloud, 
    Star, 
};

#endif  // BACKGROUNDID_H
