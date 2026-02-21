#ifndef FIREBALL_H
#define FIREBALL_H

#include "Direction.hpp"
#include "Bullet.hpp"

#include <SDL2/SDL.h>

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

class FireBall : public Bullet{
private:
    // ファイアボールの生存状態管理
    bool active = true;
    // バウンド回数計測
    int bounceCount = 0;
public:
    FireBall(double x, double y, Direction dir, Texture& tex);
    ~FireBall() = default;
    void update(double delta, const std::vector<Block>& blocks) override;
    SDL_Rect getCollisionRect() const override;
    // 生存状態関係メソッド
    // 状態確認
    bool isActive() const{
        return active;
    }
    // 非活性化
    void deactivate(){
        active = false;
    }
};

#endif  // FIREBALL_H