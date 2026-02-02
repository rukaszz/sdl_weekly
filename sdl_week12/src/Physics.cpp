#include "Physics.hpp"

/**
 * @brief 床との接地管理用のクランプ関数
 * 
 * if(vcs.vv <= 0.0)は重力加算後に呼ばれる前提の処理なので留意すること
 * vv > 0のときのみ下向き衝突判定が行われる(上昇中は何もしない)
 * 
 * @param vcs: prevFeet/newFeet/x/width/vv/onGround/ignoreDropThrough
 *     @param prevFeet: 1フレーム前のオブジェクト下部
 *     @param newFeet: 更新時のオブジェクトの下部の位置 
 * @param blocks: 描画しているブロックの情報 
 */
void Physics::resolveVerticalBlockCollision(VerticalCollisionState& vcs, const std::vector<Block>& blocks){
    if(vcs.vv <= 0.0){
        vcs.onGround = false;
        return; // 上昇中 は何もしない
    }
    // onGround監視用変数
    bool landed = false;
    // vcsの分解
    double prevFeet = vcs.prevFeet;
    double newFeet = vcs.newFeet;
    double entityLeft = vcs.x;
    double entityRight = vcs.x + vcs.width;

    // ブロック群をチェック
    for(const auto& b : blocks){
        // 乗れる床/すり抜け床以外は以下の処理をしない
        if(b.type != BlockType::Standable && b.type != BlockType::DropThrough){
            continue;
        }
        // すり抜け床かつignoreDropThroughならスキップ
        if(b.type == BlockType::DropThrough && vcs.ignoreDropThrough){
            continue;
        }
        // ブロックの下辺以外の座標
        double blockTop = b.y;
        double blockLeft = b.x;
        double blockRight = b.x + b.w;
        
        // オブジェクトがブロックの左右の辺を超えたか
        bool horizontallyOverlaps = entityRight > blockLeft
                                 && entityLeft   < blockRight;
        // オブジェクトがブロックの上辺を超えたか
        bool verticallyOverlaps = prevFeet <= blockTop
                               && newFeet  >= blockTop;

        if(horizontallyOverlaps && verticallyOverlaps){
            // 着地：ブロック上辺からオブジェクトの高さ分を補正
            vcs.newFeet = blockTop;
            vcs.vv = 0.0;
            vcs.onGround = true;
            landed = true;
            break;  // ブロックに乗った時点で抜ける
        }
    }
    // ブロック判定を抜けたときにまだ接地していないか見る
    if(!landed){
        // 垂直速度>0なら落下中
        vcs.onGround = false;
    }
}

/**
 * @brief hcsからAABBを組み立て，ブロックを走査し水平方向のめり込みを解消する
 * 
 * @param hcs: player: x/y/w/h/hv 
 * @param blocks 
 */
void Physics::resolveHorizontalBlockCollision(HorizontalCollisionState& hcs, const std::vector<Block>& blocks){
    // キャラクタの座標
    double entityLeft = hcs.x;
    double entityRight = hcs.x + hcs.width;
    double entityTop = hcs.y;
    double entityBottom = hcs.y + hcs.height;
    for(const auto& b : blocks){
        // ブロックの座標
        double blockLeft  = b.x;
        double blockRight = b.x + b.w;
        double blockTop  = b.y;
        double blockBottom = b.y + b.h;
        // AABB判定※超えていないなら何もしない
        if(entityRight <= blockLeft || blockRight <= entityLeft ||
           entityBottom <= blockTop || blockBottom <= entityTop){
            continue;
        }
        // 上下左右の重なり
        // キャラクタの右端とブロック左端の食い込み
        double overlapLeft = entityRight - blockLeft;
        // キャラクタの左端とブロック右端の食い込み
        double overlapRight = blockRight - entityLeft;
        // キャラクタの下端とブロック上端の食い込み
        double overlapTop = entityBottom - blockTop;
        // キャラクタの上端とブロック下端の食い込み
        double overlapBottom = blockBottom - entityTop;
        
        // 左右のどちらから来たか：true→左，false→右
        bool leftOrRight = overlapLeft < overlapRight;
        // 補正用変数
        // 重なっている部分の取得
        double horizontalOverlap = leftOrRight ? overlapLeft : overlapRight;
        double verticalOverlap = std::min(overlapTop,overlapBottom);
        // 矩形同士の重なりなので，重なりが浅い=本来の衝突方向とみなす
        if(horizontalOverlap < verticalOverlap){
            if(leftOrRight){
                // 左補正
                hcs.x -= overlapLeft;
            } else {
                // 右補正
                hcs.x += overlapRight;
            }
            // 水平方向の速度は0にする
            hcs.hv = 0.0;
            break;  // 1フレーム1回だけ押し戻す
        }
    }
}

void Physics::resolveFromBottom(VerticalCollisionState& vcs, const std::vector<Block>& blocks){
    // 1フレーム前の頭頂
    double prevTop = vcs.prevTop;
    // 現在のフレームの頭頂
    double newTop = vcs.newTop;
    double vv = vcs.vv;
    // 上昇中のみ
    if(vv < 0.0){
        for(const auto& b : blocks){
            // Standableのみ処理する
            if(b.type != BlockType::Standable){
                continue;
            }
            double blockBottom = b.y + b.h;
            double blockLeft = b.x;
            double blockRight = b.x + b.w;

            double entityLeft = vcs.x;
            double entityRight = vcs.x + vcs.width;

            bool horizontallyOverlaps = entityRight > blockLeft &&
                                        blockRight  > entityLeft;
            bool verticallyOverlaps = prevTop     >= blockBottom &&
                                      blockBottom >= newTop;
            if(horizontallyOverlaps && verticallyOverlaps){
                vcs.newTop = blockBottom;
                vv = 0.0;
                break;
            }
        }
    }
}
