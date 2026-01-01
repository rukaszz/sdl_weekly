#include "Renderer.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "Block.hpp"

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

    // 水平方向移動
    x += moveDir * speed * delta;
    // 垂直方向移動
    vv += PlayerConfig::PLAYER_GRAVITY * delta;
    y += vv * delta;

    // 移動後の足元の位置
    double newFeet = y + sprite.getDrawHeight();

    // clampToBounds(bounds);   // Characterのclampは使わない
    // clampToGround(bounds);
    clampToGround(prevFeet, newFeet, blocks);
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
 * @brief 床との接地管理用のクランプ関数
 * 
 * @param prevFeet: 1フレーム前のプレイヤー下部
 * @param newFeet: 更新時のプレイヤーの下部の位置 
 * @param blocks: 描画しているブロックの情報 
 */
void Player::clampToGround(double prevFeet, double newFeet, const std::vector<Block>& blocks){
    if(vv > 0.0){
        // onGround監視用変数
        bool landed = false;
        for(const auto& b : blocks){
            if(b.type != BlockType::Standable){
                continue;   // 乗れる床以外は以下の処理をしない
            }
            // ブロックの下辺以外の座標
            double blockTop = b.y;
            double blockLeft = b.x;
            double blockRight = b.x + b.w;
            // Playerの座標
            double playerLeft = x;
            double playerRight = x + sprite.getDrawWidth();
            
            // プレイヤーがブロックの左右の辺を超えたか
            bool horizontallyOverlaps = playerRight > blockLeft
                                     && playerLeft  < blockRight;
            // プレイヤーがブロックの上辺を超えたか
            bool verticallyOverlaps = prevFeet <= blockTop
                                   && newFeet  >= blockTop;

            if(horizontallyOverlaps && verticallyOverlaps){
                // 着地：ブロック上辺からplayer高さ分を補正
                y = blockTop - sprite.getDrawHeight();
                vv = 0.0;
                onGround = true;
                landed = true;
                break;  // ブロックに乗った時点で抜ける
            }
        }
        // ブロック判定を抜けたときにまだ接地していないか見る
        if(!landed){
            // 垂直速度>0なら落下中
            onGround = false;
        }
    } else {
        // 垂直速度<0なら上昇中
        onGround =false;
    }
}

/**
 * @brief x軸との接地管理用のクランプ関数
 * 
 * @param bounds 
 */
void Player::clampHorizontalPosition(const DrawBounds& bounds){
    x = std::clamp(x, 0.0, bounds.drawableWidth - sprite.getDrawWidth());
}

