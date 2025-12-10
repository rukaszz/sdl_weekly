#ifndef GAMEUTIL_H
#define GAMEUTIL_H

/**
 * @file GameUtil.hpp
 * @author rukaszz
 * @brief namespace + header function
 * @version 0.1
 * 
 */

#include <SDL2/SDL.h>

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
    inline bool intersects(const SDL_Rect a, const SDL_Rect b){       
        return !(a.x + a.w <= b.x || // 矩形aがbの左にある
                 b.x + b.w <= a.x || // 矩形bがaの左にある
                 a.y + a.h <= b.y || // 矩形aがbの上にある
                 b.y + a.h <= a.y    // 矩形bがaの上にある
                );
    }
}

#endif  // GAMEUTIL_H
