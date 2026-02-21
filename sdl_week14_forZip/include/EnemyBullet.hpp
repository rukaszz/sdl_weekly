#ifndef ENEMYBULLET_H
#define ENEMYBULLET_H

#include "Direction.hpp"
#include "Bullet.hpp"

#include <SDL2/SDL.h>

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

class EnemyBullet : public Bullet{
private:
    // ファイアボールの生存状態管理
    bool active = true;
public:
    // Bullet
    EnemyBullet(double x, double y, Direction dir, Texture& tex);
    ~EnemyBullet() = default;
    void update(double delta, const std::vector<Block>& /*blocks*/) override;
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

#endif  // ENEMYBULLET_H
