#ifndef PLAYER_H
#define PLAYER_H

// 定数
#include "PlayerConfig.hpp"

#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"

#include <SDL2/SDL.h>

#include <vector>

class Texture;
class Renderer;
struct Block;

class Player : public Character{
private:
    // 接地状態管理
    bool onGround = false;

public:
    // 定数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;

    Player(Texture& tex);

    void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) override;
    SDL_Rect getCollisionRect() const override;
    void reset();
    
    void clampToGround(double prevFeet, double newFeet, const std::vector<Block>& blocks);
    void clampHorizontalPosition(const DrawBounds& bounds);
};

#endif  // PLAYER_H
