#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Physics.hpp"

#include "Direction.hpp"
#include "DrawBounds.hpp"
#include "GameConfig.hpp"
#include "PlayerConfig.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>
#include <string>

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
        PlayerConfig::RUN_MAX_SPEED,        // speed
        0.0,                                // 水平速度
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
    // 1. 1つ前のフレームのサンプリング 
    // 頭頂(プレイヤーの最上部)の位置
    beginFrameTopPhysicsSample();
    // 足元(プレイヤーの最下部)の位置
    beginFrameFeetPhysicsSample();   // prevFeetPhisicsのサンプリング

    // 水平移動用変数
    double moveDir = 0.0;
    // 移動中以外はfalse
    bool moving = false;
    // DropThroughの判定
    bool dropThrough = false;
    // 2. 入力処理
    inputProcessing(delta, 
                    input, 
                    moveDir, 
                    moving, 
                    dropThrough);
    // 3. x/y/vvの更新
    moveElementsUpdate(delta, input, moveDir);
    // 4. 物理処理
    physicsProcessing(blocks, dropThrough);
    // 5. x軸方向のクランプ処理
    clampHorizontalPosition(bounds);
    // 6. アニメーション処理
    animationProcessing(delta, moving);
}

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Player::getCollisionRect() const{
    return {
        static_cast<int>(x) + PlayerConfig::COLLISION_MARGIN_X, 
        static_cast<int>(y) + PlayerConfig::COLLISION_MARGIN_Y,
        sprite.getDrawWidth() - PlayerConfig::COLLISION_MARGIN_X *2,
        sprite.getDrawHeight() - PlayerConfig::COLLISION_MARGIN_Y * 2};
}

/**
 * @brief プレイヤーの状態を初期状態に戻す
 * 
 */
void Player::reset(){
    // Playerの座標
    setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 水平速度
    hv = 0.0;
    // 垂直速度
    vv = 0.0;
    // 設置状態(trueにしてclampで正常にする)
    onGround = false;
    isJumping = false;
    jumpElapsed = 0.0;
    coyoteTimer = 0.0;
    jumpableBufferTimer = 0.0;
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

/**
 * @brief ジャンプボタン押下時間に応じた垂直速度設定関数
 * 
 * @param delta 
 * @param input 
 */
void Player::detectJumpButtonState(double delta, const InputState& input){
    // 重力
    double gravity = PlayerConfig::PLAYER_GRAVITY;
    // ジャンプによる上昇中のみ可変重力を設定
    if(isJumping && vv < 0.0){
        // ジャンプボタン押下時間計測
        jumpElapsed += delta;
        // ジャンプボタンが押されているか
        bool jumpButtomReleased = !input.pressed[static_cast<int>(Action::Jump)];
        // ジャンプボタンが規定時間以上押されているか
        bool overMaxHold = (jumpElapsed >= PlayerConfig::JUMP_HOLD_MAX_TIME);
        // ジャンプボタンを離した or 押下時間の上限に達したら通常の重力を適用
        if(jumpButtomReleased || overMaxHold){
            // 通常の重力
            isJumping = false;
        } else {
            // ジャンプボタンを押し続けているときは重力を弱くする(高く飛べる)
            gravity *= 0.2;
        }
    } 
    // 重力の加算は最後に加算
    vv += gravity * delta;
}

/**
 * @brief 入力の読み取りとその処理
 * 各キーの押下に応じた処理(変数の設定)を実施する
 * 
 * @param delta 
 * @param input 
 * @param moveDir 
 * @param moving 
 * @param dropThrough 
 */
void Player::inputProcessing(double delta, 
                             const InputState& input, 
                             double& moveDir, 
                             bool& moving, 
                             bool& dropThrough)
{
    if(input.pressed[static_cast<int>(Action::MoveLeft)]){
        moveDir -= 1.0;
        dir = Direction::Left;
        moving = true;
    }
    if(input.pressed[static_cast<int>(Action::MoveRight)]){
        moveDir += 1.0;
        dir = Direction::Right;
        moving = true;
    }
    // Jump
    if(input.justPressed[static_cast<int>(Action::Jump)]){
        jumpableBufferTimer = PlayerConfig::JUMPABLE_BUFFER_TIME;
    } else {
        jumpableBufferTimer = std::max(0.0, jumpableBufferTimer - delta);
    }
    // コヨーテタイマー
    if(onGround){
        coyoteTimer = PlayerConfig::COYOTE_TIME;
    }else{
        coyoteTimer = std::max(0.0, coyoteTimer - delta);
    }
    // ジャンプ可能条件
    const bool canUseCoyote = (onGround || coyoteTimer > 0.0);
    const bool hasBufferJump = (jumpableBufferTimer > 0.0);
    // ジャンプ動作
    if(canUseCoyote && hasBufferJump){
        vv = -PlayerConfig::JUMP_VELOCITY;  // 上方向はy軸的には負
        onGround = false;
        isJumping = true;
        jumpElapsed = 0.0;
        coyoteTimer = 0.0;  // ジャンプの猶予は0にする
        // jumpableBufferTimerはjustPressedがfalseになったら減衰して0になるが，ここで明示的に0にしておく
        jumpableBufferTimer = 0.0;
    }
    // DropThrough：接地中 かつ 下を押下
    if(onGround && input.pressed[static_cast<int>(Action::MoveDown)]){
        onGround = false;   // onGroundを外す
        dropThrough = true;
        coyoteTimer = 0.0;  // すり抜け時のジャンプは許可しない

    }
}

/**
 * @brief x/vv/yを更新する関数
 * 
 * @param delta 
 * @param input 
 * @param moveDir 
 */
void Player::moveElementsUpdate(double delta, const InputState& input, const double moveDir){
    // 物理の更新
    // 水平方向移動
    if(moveDir != 0.0){
        hv += moveDir * PlayerConfig::ACCELERATION *delta;
        hv = std::clamp(hv, -speed, speed);
    } else {
        // 入力がないなら摩擦で減速する
        if(hv > 0.0){
            hv = std::max(0.0, hv - PlayerConfig::FRICTION * delta);
        } else if(hv < 0.0){
            hv = std::min(0.0, hv + PlayerConfig::FRICTION * delta);
        }
    }
    x += hv * delta;

    // 垂直方向移動
    // 可変ジャンプ
    detectJumpButtonState(delta, input);
    // detectJumpButtonState()でvvを更新からyを更新
    y += vv * delta;
}

/**
 * @brief 物理処理の呼び出しと状態の更新
 * 物理処理の順番は縦方向の補正→横方向の補正
 * 
 * @param blocks 
 * @param dropThrough 
 */
void Player::physicsProcessing(const std::vector<Block>& blocks, const bool dropThrough){
    // 移動後の(このフレームの)頭頂の位置の計算
    double newTop = y;
    // 移動後の(このフレームの)足元の位置の計算
    const double sprite_H = static_cast<double>(sprite.getDrawHeight());
    double newFeet = y + sprite_H;

    // 縦方向の補正
    // DTO作成
    VerticalCollisionState vcs{
        .prevTop  = getPrevTopPhysics(), 
        .newTop   = newTop, 
        .prevFeet = getPrevFeetPhysics(),  // 処理の最初で保存したprevFeetを参照
        .newFeet  = newFeet,
        .x        = x,
        .width    = static_cast<double>(sprite.getDrawWidth()),
        .vv       = vv,
        .onGround = onGround, 
        .ignoreDropThrough = dropThrough
    };
    // 垂直速度で判定を分岐する
    if (vv > 0.0) {
        // 落下：床判定だけ
        Physics::resolveBlockCollisionFromTop(vcs, blocks);
        y        = vcs.newFeet - sprite_H;
        vv       = vcs.vv;
        onGround = vcs.onGround;
    } else if (vv < 0.0) {
        // 上昇：天井判定だけ
        Physics::resolveBlockCollisionFromBottom(vcs, blocks);
        y  = vcs.newTop;
        vv = vcs.vv;
        // onGround は false のまま
    } else {
        // vv == 0.0 → 何もしない
    }

    // 横方向の補正
    SDL_Rect col = getCollisionRect();
    HorizontalCollisionState hcs{
        .x      = static_cast<double>(col.x), 
        .y      = static_cast<double>(col.y), 
        .width  = static_cast<double>(col.w),
        .height = static_cast<double>(col.h),
        .hv     = hv
    };
    Physics::resolveHorizontalBlockCollision(hcs, blocks);
    // 帰ってきた結果をPlayer内部へ反映
    x = hcs.x - PlayerConfig::COLLISION_MARGIN_X;   // getCollisionRectのオフセット(現状はハードコード)
    hv = hcs.hv;

    // 状態の整理
    if(onGround && vv >= 0.0){
        // 頭をぶつけたようなvv>=0のタイミングで状態がちぐはぐになるのを防止
        isJumping = false;
        jumpElapsed = 0.0;
    }
}

/**
 * @brief アニメーション更新関数
 * 
 * @param delta 
 * @param moving 
 */
void Player::animationProcessing(double delta, const bool moving){
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
 * @brief デバッグ用テキスト出力関数
 * PlayingSceneで画面に文字を表示するために文字列を構成する
 * 
 * @return std::string 
 */
std::string Player::debugMoveContext(){
    std::string mvCtx = "";
    mvCtx = "hv: "      + std::to_string(hv)
          + "vv: "      + std::to_string(vv)
          + "onG: "     + std::to_string(onGround)
          + "cyt: "     + std::to_string(coyoteTimer)
          + "jbt: "     + std::to_string(jumpableBufferTimer)
          + "isJ: "     + std::to_string(isJumping);
    return mvCtx;
}
