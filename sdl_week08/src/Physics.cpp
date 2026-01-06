#include "Physics.hpp"
#include "PlayerConfig.hpp"

/**
 * @brief 床との接地管理用のクランプ関数
 * 
 * @param prevFeet: 1フレーム前のオブジェクト下部
 * @param newFeet: 更新時のオブジェクトの下部の位置 
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
    double playerLeft = vcs.x;
    double playerRight = vcs.x + vcs.width;

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
        bool horizontallyOverlaps = playerRight > blockLeft
                                 && playerLeft  < blockRight;
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
