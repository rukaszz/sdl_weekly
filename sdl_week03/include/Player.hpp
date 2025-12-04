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

public:
    // 定数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;

    Player(Texture& tex);

    void update(double delta, DrawBounds bounds) override;
    void draw(Renderer& renderer) override;
    const Sprite& getSprite() const override;

};

#endif  // PLAYER_H