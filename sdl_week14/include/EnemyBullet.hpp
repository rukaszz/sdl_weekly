#ifndef ENEMYBURRET_H
#define ENEMYBURRET_H

#include "Direction.hpp"
#include "Bullet.hpp"

#include <SDL2/SDL.h>

#include <vector>

class Texture;
class Renderer;
struct Block;
struct Camera;

class EnemyBurret : public Bullet{
private:
    // ファイアボールの生存状態管理
    bool active = true;
    // バウンド回数計測
    int bounceCount = 0;
public:
    EnemyBurret(double x, double y, Direction dir, Texture& tex);
    ~EnemyBurret() = default;
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

#endif  // ENEMYBURRET_H
