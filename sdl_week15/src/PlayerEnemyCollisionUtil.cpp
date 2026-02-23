#include "PlayerEnemyCollisionUtil.hpp"
#include "PlayerConfig.hpp"
#include "GameUtil.hpp"

namespace PlayerEnemyCollision{
    /**
     * @brief 敵の踏みつけ処理
     * 
     * 座標系はワールド座標系(カメラを考慮していない)
     * AABB&Rectによる判定後，描画サイズ基準の小さいあたり判定を行う
     * 
     * @param playerRect 
     * @param playerPrevFeet 
     * @param playerNewFeet 
     * @param playerVv 
     * @param enemyRect 
     * @return PlayerEnemyCollisionResult 
     */
    PlayerEnemyCollisionResult resolvePlayerEnemyCollision(
        const SDL_Rect& playerRect, 
        double playerPrevFeet, 
        double playerNewFeet, 
        double playerVv,
        const SDL_Rect& enemyRect 
    )
    {
        // 1. そもそも接触しているか=接触していたら以下の処理が走る
        if (!GameUtil::intersects(playerRect, enemyRect)) {
            return PlayerEnemyCollisionResult::None;
        }
        // 敵の頭上
        const double enemyTop = static_cast<double>(enemyRect.y);
        // 落ちているか
        const bool falling = (playerVv > 0.0);
        // 敵の頭とプレイヤーの足が接触したか
        const bool feetCrossTop = (playerPrevFeet <= enemyTop + PlayerConfig::eps && enemyTop <= playerNewFeet + PlayerConfig::eps);

        // 2. 落下中に敵の頭をまたいでいたらstomp
        if(falling && feetCrossTop){
            return PlayerEnemyCollisionResult::StompEnemy;
        }
        // 3. 2の接触以外は全てPlayerHit
        return PlayerEnemyCollisionResult::PlayerHit;
    }
}   // PlayerEnemyCollision