#ifndef FIREBALL_H
#define FIREBALL_H

#include "Direction.hpp"
#include "DrawBounds.hpp"
#include "Bullet.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"
#include "Input.hpp"

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

class FireBall : public Bullet{
private:

public:
    FireBall(double x, double y, Direction dir, Texture& tex);
    ~FireBall();
    void update(double delta, const std::vector<Block>& blocks) override;
    SDL_Rect getCollisionRect() const override;
};

#endif  // FIREBALL_H