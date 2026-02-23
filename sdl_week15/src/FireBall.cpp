#include "FireBall.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Block.hpp"

#include "FireBallConfig.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

/**
 * @brief Construct a new Fire Ball:: Fire Ball object
 * 
 * @param x 
 * @param y 
 * @param dir 
 * @param tex 
 */
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
        FireBallConfig::ANIM_INTERVAL  // animInterval
    )
{
    sprite.setFrame(0);
}

/**
 * @brief ファイアボールの挙動を設定
 * 現状は床(上から下)とのバウンドがメイン
 * 
 * @param delta 
 * @param blocks 
 */
void FireBall::update(double delta, const std::vector<Block>& blocks){
    // 死んだ弾は何もしない
    if(!active){
        return;
    }
    // 1. 水平移動
    x += hv * delta;

    // 2. 垂直移動
    const double h = static_cast<double>(sprite.getDrawHeight());
    double prevBottom = y + h;

    vv += FireBallConfig::GRAVITY * delta;
    double newBottom = prevBottom + vv * delta;

    double entityLeft = x;
    double entityRight = x + sprite.getDrawWidth();

    bool bounced = false;
    // 最も近くの床判定用y座標
    double bestFloor_Y = std::numeric_limits<double>::infinity();

    // 下向きに移動しているときだけ床との接触判定を実施
    if(vv > 0.0){
        // 各ブロックとの判定
        for(const auto& b : blocks){
            // 床タイプ以外のブロックとは処理しない
            if(b.type != BlockType::Standable){
                // deactivate();
                continue;
            }
            // ブロックの要素
            double blockTop = b.y;
            double blockLeft = b.x;
            double blockRight = b.x + b.w;
            // 垂直方向の重なり
            bool horizontallyOverlaps = (entityRight > blockLeft) &&
                                        (blockRight  > entityLeft);
            // 前フレームのFireBallのbottomが床より上 && 現在のフレームのbottomが床より下
            bool crossedFloor = (blockTop  >= prevBottom) &&
                                (newBottom >= blockTop);
            // 垂直方向で重なっている && ブロックにめり込んでいる
            if(horizontallyOverlaps && crossedFloor){
                // 最も近い床に当たったらそのy座標を採用する(近いものに更新されていく)
                if(blockTop < bestFloor_Y){
                    bestFloor_Y = blockTop;
                    bounced = true;
                }
            }
        }
    }
    // 床に接触したら
    if(bounced){
        // 床の上に乗るようにbottomを補正
        y = bestFloor_Y - h;
        // 速度の反転と減衰を入れる
        vv = -vv * FireBallConfig::BOUNCE;
        ++bounceCount;
        // バウンドの減衰がある程度入った or 6回以上のバウンドで非活性化
        if(std::abs(vv) < 50.0 || bounceCount >= FireBallConfig::MAX_BOUNCE){
            deactivate();
        }
    } else {
        // 床と接触していないときは通常のy軸の更新
        y += vv * delta;
    }

    // 3. アニメーション更新
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief 当たり判定用の矩形を返却する関数
 * 
 * @return SDL_Rect 
 */
SDL_Rect FireBall::getCollisionRect() const{
    return {
        static_cast<int>(x),
        static_cast<int>(y),
        sprite.getDrawWidth(),
        sprite.getDrawHeight()
    };
}
