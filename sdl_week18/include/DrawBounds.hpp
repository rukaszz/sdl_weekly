#ifndef DRAWBOUNDS_H
#define DRAWBOUNDS_H

// 描画境界
struct DrawBounds{
    double minX = 0.0;      // ボス戦などで制限を設ける用
    double drawableWidth;
    double drawableHeight;
};

#endif  // DRAWBOUNDS_H
