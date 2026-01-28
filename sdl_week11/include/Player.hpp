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
    // ジャンプ状態管理
    bool isJumping = false;     // ジャンプ中か
    double jumpElapsed = 0.0;   // ジャンプボタン押下からの経過時間
    // コヨーテタイム
    // ※落下判定の初期段階でジャンプを許可するやつ
    double coyoteTimer = 0.0;
    static inline constexpr double COYOTE_TIME = 0.08;  // 約5フレーム(0.016*5)
public:
    // 定数
    // アニメーション枚数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;
    // ジャンプボタン押下時の押しっぱなしの時間の長さ
    static inline constexpr double JUMP_HOLD_MAX_TIME = 0.3;    // 300ms程度の猶予
    Player(Texture& tex);

    void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) override;
    SDL_Rect getCollisionRect() const override;
    void reset();
    
    void clampHorizontalPosition(const DrawBounds& bounds);
    void detectJumpButtonState(double delta, const InputState& input);
};

#endif  // PLAYER_H
