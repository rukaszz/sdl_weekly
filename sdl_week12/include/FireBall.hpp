#ifndef FIREBALL_H
#define FIREBALL_H

#include "Bullet.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"
#include "Input.hpp"

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

// 投射物の向き
enum class Direction{
    Left, 
    Right
};

// 描画境界
struct DrawBounds{
    double drawableWidth;
    double drawableHeight;
};

// キャラクタのサイズ(矩形)
struct BulletBounds
{
    double x, y, w, h;
};


class FireBall : public Bullet{
protected:
    // 画面の座標(左上が0, 0)
    double x, y;
    // 水平速度(horizontal verocity)
    double hv;
    // 垂直速度(vertical verocity)
    double vv;
    // 向き
    Direction dir;
    // アニメーション管理
    AnimationController anim;
    // スプライト管理
    Sprite sprite;

public:
    FireBall(
        double x, double y,
        double hv,
        double vv,
        Direction dir,
        Texture& tex,
        int frameW, int frameH,
        int maxFrames,
        double animInterval
    );
    ~FireBall();
    void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) = 0;
    SDL_Rect getCollisionRect() const = 0;

};

#endif  // FIREBALL_H