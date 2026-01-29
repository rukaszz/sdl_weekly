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
public:
    // 定数
    // アニメーション枚数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;
    // ジャンプボタン押下時の押しっぱなしの時間の長さ
    Player(Texture& tex);

    void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) override;
    SDL_Rect getCollisionRect() const override;
    void reset();
    
    // update()関係
    void inputProcessing(double delta, 
                         const InputState& input, 
                         double& moveDir, 
                         bool& moving, 
                         bool& dropThrough);
    void moveElementsUpdate(double delta, const InputState& input, const double moveDir);
    void detectJumpButtonState(double delta, const InputState& input);
    void physicsProcessing(const std::vector<Block>& blocks, const bool dropThrough);
    void clampHorizontalPosition(const DrawBounds& bounds);
    void animationProcessing(double delta, const bool moving);
};

#endif  // PLAYER_H
