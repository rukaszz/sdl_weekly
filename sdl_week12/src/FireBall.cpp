#include "Bullet.hpp"
#include "FireBall.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

#include <algorithm>

FireBall::FireBall(
    double x_, double y_,
    double hv_,
    double vv_,
    Direction dir_,
    Texture& tex,
    int frameW, int frameH,
    int maxFrames,
    double animInterval
)
    : x(x_)
    , y(y_)
    , hv(hv_)
    , vv(vv_)
    , dir(dir_)
    , anim(maxFrames, animInterval)
    , sprite(tex, frameW, frameH)
{

}

/**
 * @brief Destroy the FireBall:: FireBall object
 * 
 */
FireBall::~FireBall() = default;

FireBall::update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks);

FireBall::getCollisionRect()


