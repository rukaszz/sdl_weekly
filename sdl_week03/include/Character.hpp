#ifndef CHARACTER_H
#define CHARACTER_H

#include "AnimationController.hpp"
#include "Sprite.hpp"

class Texture;
class Renderer;

enum class Direction{
    Left, 
    Right
};

class Character{
protected:
    // 画面の座標(左上が0, 0)
    double x, y;
    // 移動速度
    double speed;
    // 向き
    Direction dir;
    // アニメーション管理
    AnimationController anim;
    // スプライト管理
    Sprite sprite;

public:
    Character(
        double x, double y,
        double speed,
        Direction dir,
        Texture& tex,
        int frameW, int frameH,
        int maxFrames,
        double animInterval
    );
    virtual ~Character();
    virtual void update(double delta, SDL_Point drawableSize) = 0;
    virtual void draw(Renderer& renderer);
    
};

#endif  // CHARACTER_H