#ifndef DRAWBOUNDS_H
#define DRAWBOUNDS_H

// 描画境界
struct DrawBounds{
    // ボス戦などで制限を設ける用
    double min_X = 0.0;
    double min_Y = 0.0;
    // 描画の端
    double max_X;
    double max_Y;
};

/*
struct DrawBounds{
    double minX = 0.0;      // ボス戦などで制限を設ける用
    double drawableWidth;
    double drawableHeight;
};
*/

#endif  // DRAWBOUNDS_H
