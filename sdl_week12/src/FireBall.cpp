#include "Bullet.hpp"
#include "FireBall.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

#include "FireBallConfig.hpp"

#include <algorithm>

FireBall::FireBall(double x, double y, Direction dir, Texture& tex)
    : Bullet(
        x, y,   // x, y
        (dir == Direction::Right ? FireBallConfig::SPEED_X : -FireBallConfig::SPEED_X),     // hv
        0.0,    // vv
        dir,    //dir
        tex,    // tex
        FireBallConfig::FRAME_W,    // frameW 
        FireBallConfig::FRAME_H,    // frameH
        FireBallConfig::NUM_FRAMES, // maxFrames    
        FireBallConfig::ANIM_INTERVAVL  // animInterval
    )
{
    sprite.setFrame(0);
}

/**
 * @brief Destroy the FireBall:: FireBall object
 * 
 */
FireBall::~FireBall() = default;

void FireBall::update(double delta, DrawBounds bounds, const std::vector<Block>& blocks){
    // TODO: FireBallの更新
}

SDL_Rect FireBall::getCollisionRect() const{
    // TODO: 当たり判定用の矩形を返却
    return SDL_Rect{0, 0, 0, 0};
}


