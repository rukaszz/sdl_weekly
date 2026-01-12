#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Physics.hpp"

#include "GameConfig.hpp"
#include "PlayerConfig.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

/**
 * Playerクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 */

/**
 * @brief Construct a new Player:: Player object
 * 
 * @param tex 
 */
Player::Player(Texture& tex)
    : Character(
        PlayerConfig::POS_X, 
        PlayerConfig::POS_Y,                // 初期座標
        200.0,                              // speed
        0.0,                                // 垂直速度
        Direction::Right,                   // dir
        tex,                                // texture
        PlayerConfig::FRAME_W, 
        PlayerConfig::FRAME_H,
        PlayerConfig::NUM_FRAMES,           // maxFrames
        0.1                                 // animInterval
    )
{
    sprite.setFrame(0);
}

/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * ジャンプを実装するので上下移動はなくなる
 * 
 * @param delta: 差分 
 * @param input: Inputクラスの状態=キー入力 
 * @param bounds: 描画可能範囲 
 */
void Player::update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks){
    // 水平移動用変数
    double moveDir = 0.0;
    // 1つ前のフレームの位置(yのみ)
    double prev_Y = y;
    // 1つ前の足元(プレイヤーの最下部)の位置
    double prevFeet = prev_Y + sprite.getDrawHeight();

    // 移動中以外はfalse
    bool moving = false;
    // DropThroughの判定
    bool dropThrough = false;
    
    if(input.pressed[(int)Action::MoveLeft]){
        // x -= speed * delta;
        moveDir -= 1.0;
        dir = Direction::Left;
        moving = true;
    }
    if(input.pressed[(int)Action::MoveRight]){
        // x += speed * delta;
        moveDir += 1.0;
        dir = Direction::Right;
        moving = true;
    }
    // Jump
    if(onGround && input.justPressed[(int)Action::Jump]){
        vv = -PlayerConfig::JUMP_VELOCITY;  // 上方向はy軸的には負
        onGround = false;
    }
    // DropThrough：接地中 かつ 下を押下
    if(onGround && input.pressed[(int)Action::MoveDown]){
        onGround = false;   // onGroundを外す
        dropThrough = true;
    }

    // 水平方向移動
    x += moveDir * speed * delta;
    // 垂直方向移動
    vv += PlayerConfig::PLAYER_GRAVITY * delta;
    y += vv * delta;

    // 移動後の足元の位置
    double newFeet = y + sprite.getDrawHeight();

    /* -----Physicsへ物理的な挙動を移管 ----- */
    // clampToGround(prevFeet, newFeet, blocks);
    // DTO作成
    VerticalCollisionState vcs{
        .prevFeet = prevFeet,
        .newFeet  = newFeet,
        .x        = x,
        .width    = static_cast<double>(sprite.getDrawWidth()),
        .vv       = vv,
        .onGround = onGround, 
        .ignoreDropThrough = dropThrough
    };
    Physics::resolveVerticalBlockCollision(vcs, blocks);
    // 帰ってきた結果をPlayerの内部へ反映する
    y        = vcs.newFeet - sprite.getDrawHeight();
    vv       = vcs.vv;
    onGround = vcs.onGround;
    /* -----Physicsへ物理的な挙動を移管 ----- */
    clampHorizontalPosition(bounds);

    // アニメーション処理
    if(!moving){
        anim.reset();
        sprite.setFrame(anim.getFrame());
        return;
    }
    // フレームを動かして描画
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Player::getCollisionRect() const{
    return {(int)x+10, (int)y+10, sprite.getDrawWidth()-20, sprite.getDrawHeight()-20};
}

/**
 * @brief プレイヤーの状態を初期状態に戻す
 * 
 */
void Player::reset(){
    // Playerの座標
    setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 垂直速度
    vv = 0.0;
    // 設置状態(trueにしてclampで正常にする)
    onGround = false;
    // アニメーションリセット
    anim.reset();
}

/**
 * @brief x軸との接地管理用のクランプ関数
 * 
 * @param bounds 
 */
void Player::clampHorizontalPosition(const DrawBounds& bounds){
    x = std::clamp(x, 0.0, bounds.drawableWidth - sprite.getDrawWidth());
}

