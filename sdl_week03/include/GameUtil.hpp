#ifndef GAMEUTIL_H
#define GAMEUTIL_H

/**
 * @file GameUtil.hpp
 * @author rukaszz
 * @brief namespace + header function
 * @version 0.1
 * 
 */

#include "Sprite.hpp"

/**
 * @brief Axis-Aligned Bounding Box(AABB)衝突判定関数
 * 矩形の点がもう一方の矩形の内部にあるかを調べ，衝突判定をする
 * 
 * @param a: 対象aのスプライト 
 * @param b: 対象bのスプライト 
 * @return true: 衝突 
 * @return false 
 */
namespace GameUtil{
    inline bool intersects(const Sprite& a, const Sprite& b){
        const auto& ra = a.getDstRect();
        const auto& rb = b.getDstRect();
        
        return !(ra.x + ra.w <= rb.x || // 矩形aがbの左にある
                 rb.x + rb.w <= ra.x || // 矩形bがaの左にある
                 ra.y + ra.h <= rb.y || // 矩形aがbの上にある
                 rb.y + ra.h <= ra.y    // 矩形bがaの上にある
                );
    }
}

#endif  // GAMEUTIL_H
