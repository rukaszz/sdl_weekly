#ifndef UICONFIG_H
#define UICONFIG_H

#include <SDL2/SDL.h>

namespace UIConfig{
    struct BossHpBarConfig{
        // BAR_Xは計算で導出する
        static inline constexpr int BAR_Y = 50;         // y座標
        static inline constexpr int BAR_W = 350;        // 幅
        static inline constexpr int BAR_H = 20;         // 高さ
        static inline constexpr int FRAME_OFFSET = 5;   // 枠用の遊び
        // HPバーの色定義
        // フレーム：白
        static inline constexpr SDL_Color FRAME_COLOR     = {255, 255, 255, 255};
        // 背景：灰
        static inline constexpr SDL_Color BG_COLOR        = {64, 64, 64, 255};
        // 高HP：緑
        static inline constexpr SDL_Color HIGH_HP_COLOR   = {40, 220, 40, 255};
        // 中HP：黃
        static inline constexpr SDL_Color MIDDLE_HP_COLOR = {240, 200, 40, 255};
        // 低HP：赤
        static inline constexpr SDL_Color LOW_HP_COLOR    = {220, 40, 40, 255};
    };
} // namespace UIConfig

#endif  // UICONFIG_H
