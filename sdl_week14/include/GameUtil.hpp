#ifndef GAMEUTIL_H
#define GAMEUTIL_H

/**
 * @file GameUtil.hpp
 * @author rukaszz
 * @brief namespace + header function
 * @version 0.1
 * 
 */

#include "Block.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cctype>
#include <string>

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
                 b.y + b.h <= a.y    // 矩形bがaの上にある
                );
    }


    /**
     * @brief Blockの値をSDL_Rectへ変換する関数
     * SDL_Rect型で返すのでintでキャストしている
     * 
     * @param b 
     * @return SDL_Rect 
     */
    inline SDL_Rect blockToRect(const Block& b){
        return SDL_Rect{
            static_cast<int>(b.x),
            static_cast<int>(b.y),
            static_cast<int>(b.w),
            static_cast<int>(b.h)
        };
    }

    /**
     * @brief 空白のトリムを行う関数
     * すべて空白のパターンはtrim後のline.empty()で弾く
     * 全角空白などは考慮していない
     * 
     * @param s 
     */
    inline void trim(std::string& s){
        // 先頭の空白の削除
        // 先頭の空白を見つける
        auto hs = std::find_if(s.begin(), s.end(), [](unsigned char ch){ return !std::isspace(ch); });
        s.erase(s.begin(), hs);
        // 末尾の空白の削除
        // 末尾の空白を見つける
        auto ts = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base();
        s.erase(ts, s.end());
    }

    /**
     * @brief EntityのSDL_Rectを受け取り，Worldの外にいるかを判定する関数
     * 
     * @param r 
     * @param world_W 
     * @param world_H 
     * @param margin_W 
     * @param margin_H 
     * @return true 
     * @return false 
     */
    inline bool isOutOfWorldBounds(SDL_Rect r, 
                                   double world_W, double world_H,
                                   double margin_W, double margin_H
                                  )
    {
        // SDL_Rectの分解
        double top    = r.y;
        double bottom = r.y + r.h;
        double right  = r.x + r.w;
        double left   = r.x;

        if(bottom < 0.0 - margin_H) return true;
        if (top    > world_H + margin_H)  return true;
        if (right  < 0.0 - margin_W)      return true;
        if (left   > world_W + margin_W)  return true;

        return false;
    }
}

#endif  // GAMEUTIL_H
