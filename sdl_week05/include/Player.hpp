#ifndef PLAYER_H
#define PLAYER_H

// 定数
#include "PlayerConfig.hpp"

#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>

class Texture;
class Renderer;

class Player : public Character{
private:
    // 接地状態管理
    bool onGround = false;

public:
    // 定数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;

    Player(Texture& tex);

    void update(double delta, const InputState& input, DrawBounds bounds) override;
    SDL_Rect getCollisionRect() const override;
    void reset();
    void clampToGround(const DrawBounds& bounds);
    void clampHorizontalPosition(const DrawBounds& bounds);
};

#endif  // PLAYER_H
