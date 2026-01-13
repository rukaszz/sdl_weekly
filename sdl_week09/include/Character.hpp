#ifndef CHARACTER_H
#define CHARACTER_H

#include "AnimationController.hpp"
#include "Sprite.hpp"
#include "Input.hpp"

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

// キャラクタの向き
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
struct CharaBounds
{
    double x, y, w, h;
};


class Character{
protected:
    // 画面の座標(左上が0, 0)
    double x, y;
    // 移動速度
    double speed;
    // 1フレーム前のRect下部
    double prevFeet;
    // 垂直速度(vertical verocity)
    double vv;
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
        double vv,
        Direction dir,
        Texture& tex,
        int frameW, int frameH,
        int maxFrames,
        double animInterval
    );
    virtual ~Character();
    virtual void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) = 0;
    virtual SDL_Rect getCollisionRect() const = 0;
    // virtual void draw(Renderer& renderer);
    virtual void draw(Renderer& renderer, Camera& camera);
    virtual double getEntityCenter_X();
    virtual double getEntityCenter_Y(); 
    virtual const Sprite& getSprite() const;
    virtual void setPosition(double coorX, double coorY);
    virtual void clampToBounds(const DrawBounds& b);

    // getter/setter
    double getPrevFeet() const{
        return prevFeet;
    }
    void setPrevFeet(double v){
        prevFeet = v;
    }
    double getFeet() const{
        SDL_Rect r = getCollisionRect();
        return r.y + r.h;
    }
    void beginFrameFeetSample(){
        prevFeet = getFeet();
    }
    double getVerticalVelocity() const{
        return vv;
    }
    void setVerticalVelocity(double v){
        vv = v;
    }
};

#endif  // CHARACTER_H