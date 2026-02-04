#ifndef BULLET_H
#define BULLET_H

#include "Direction.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

class Bullet{
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
    Bullet(
        double x, double y,
        double hv,
        double vv,
        Direction dir,
        Texture& tex,
        int frameW, int frameH,
        int maxFrames,
        double animInterval
    );
    virtual ~Bullet();
    virtual void update(double delta, const std::vector<Block>& blocks) = 0;
    virtual void draw(Renderer& renderer, Camera& camera);

    virtual SDL_Rect getCollisionRect() const = 0;
    virtual const Sprite& getSprite() const;
    virtual void setPosition(double coorX, double coorY);

    // getter/setter
    // 垂直速度
    double getVerticalVelocity() const{
        return vv;
    }
    void setVerticalVelocity(double v){
        vv = v;
    }
    // 水平速度
    double getHorizontalVelocity() const{
        return hv;
    }
    void setHorizontalVelocity(double v){
        hv = v;
    }
};

#endif  // BULLET_H