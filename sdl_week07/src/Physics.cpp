#include "Physics.hpp"
#include "Player.hpp"

/**
 * @brief 床との接地管理用のクランプ関数
 * 
 * @param prevFeet: 1フレーム前のプレイヤー下部
 * @param newFeet: 更新時のプレイヤーの下部の位置 
 * @param blocks: 描画しているブロックの情報 
 */
void Physics::resolveVerticalBlockCollision(Player& player, double prevFeet, double newFeet, const std::vector<Block>& blocks){
    if(player.vv > 0.0){
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
            double playerLeft = player.x;
            double playerRight = player.x + player.sprite.getDrawWidth();
            
            // プレイヤーがブロックの左右の辺を超えたか
            bool horizontallyOverlaps = playerRight > blockLeft
                                     && playerLeft  < blockRight;
            // プレイヤーがブロックの上辺を超えたか
            bool verticallyOverlaps = prevFeet <= blockTop
                                   && newFeet  >= blockTop;

            if(horizontallyOverlaps && verticallyOverlaps){
                // 着地：ブロック上辺からplayer高さ分を補正
                player.y = blockTop - player.sprite.getDrawHeight();
                player.vv = 0.0;
                player.onGround = true;
                landed = true;
                break;  // ブロックに乗った時点で抜ける
            }
        }
        // ブロック判定を抜けたときにまだ接地していないか見る
        if(!landed){
            // 垂直速度>0なら落下中
            player.onGround = false;
        }
    } else {
        // 垂直速度<0なら上昇中
        player.onGround =false;
    }
}