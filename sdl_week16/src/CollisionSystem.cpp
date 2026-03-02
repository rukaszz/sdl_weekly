#include "CollisionSystem.hpp"

#include "GameScene.hpp"
#include "SceneControl.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Block.hpp"
#include "WorldInfo.hpp"

#include <SDL2/SDL.h>   // TODO：SDL_Rectは後で取り除く

#include "GameUtil.hpp"
#include "PlayerEnemyCollisionUtil.hpp"

#include "PlayerConfig.hpp"
#include "EnemyConfig.hpp"

/**
 * @brief Construct a new Collision System:: Collision System object
 * 
 * @param player_ 
 * @param enemies_ 
 * @param blocks_ 
 * @param world_ 
 */
CollisionSystem::CollisionSystem(
    Player& player_,
    std::vector<std::unique_ptr<Enemy>>& enemies_, 
    const std::vector<Block>& blocks_, 
    const std::vector<SDL_Rect>& blockRects_,
    const WorldInfo& world_
)
    : player(player_)
    , enemies(enemies_)
    , blocks(blocks_)
    , blockRects(blockRects_)
    , world(world_)
{

}

/**
 * @brief 描画するオブジェクトの衝突処理をまとめる関数
 * 
 * 必ず呼び出す前にprevFeetCollisionをサンプリングしている必要がある
 */
void CollisionSystem::resolve(SceneControl& ctrl){
    // 敵との衝突判定
    resolveEnemyCollision(ctrl);
    // ダメージブロックとの衝突判定
    resolveBlockCollision(ctrl);
}

/**
 * @brief ブロックとの衝突処理用
 * 
 */
void CollisionSystem::resolveBlockCollision(SceneControl& ctrl){
    // ダメージブロックとの衝突判定
    SDL_Rect playerRect = player.getCollisionRect();
    // block→BlockType用，blockRect→当たり判定用
    for(std::size_t i = 0; i < blocks.size(); ++i){
        // blocks取得
        const auto& b = blocks[i];
        // ダメージ床, クリアオブジェクト以外は判定しない
        if(b.type != BlockType::Damage && b.type != BlockType::Clear){
            continue;   
        }
        // blocksの矩形取得
        const SDL_Rect& br = blockRects[i];
        // 衝突しているかをまず判定(Rect)
        if(!GameUtil::intersects(playerRect, br)){
            continue;
        }
        if(b.type == BlockType::Damage){
            ctrl.requestScene(GameScene::GameOver);
        }else if(b.type == BlockType::Clear){
            // ステージ遷移
            ctrl.requestScene(GameScene::Clear);
        }
        return;
    }
}

/*
void CollisionSystem::resolveBlockCollision(SceneControl& ctrl){
    // ダメージブロックとの衝突判定
    SDL_Rect playerRect = player.getCollisionRect();
    for(const auto& b : blocks){
        // ダメージ床, クリアオブジェクト以外は判定しない
        if(b.type != BlockType::Damage && b.type != BlockType::Clear){
            continue;   
        }
        SDL_Rect br = GameUtil::blockToRect(b);
        // 衝突しているかをまず判定(Rect)
        if(!GameUtil::intersects(playerRect, br)){
            continue;
        }
        if(b.type == BlockType::Damage){
            ctrl.requestScene(GameScene::GameOver);
        }else if(b.type == BlockType::Clear){
            // ステージ遷移
            ctrl.requestScene(GameScene::Clear);
        }
        return;
    }
}

*/

/**
 * @brief 敵との衝突用の処理
 * 敵の踏みつけ/削除/スコア加算/GameOverの処理を行う
 * 
 */
void CollisionSystem::resolveEnemyCollision(SceneControl& ctrl){
    // プレイヤーのRect基準のあたり判定取得
    // ※update→Physics →clamp→getCollisionRectの順番が必要
    SDL_Rect playerRect = player.getCollisionRect();
    // 衝突処理用変数：collisionのfeetを使う
    double prevFeet = player.getPrevFeetCollision();
    double newFeet  = player.getFeetCollision();
    double playerVv = player.getVerticalVelocity();
    
    // 接触のループ
    for(std::size_t i = 0; i < enemies.size(); ++i){
        auto& e = enemies[i];
        // 生きている敵のみ対象(Dying/Deadは除外)
        if(!e->isAlive()){
            continue;
        }
        // 矩形の当たり判定取得
        SDL_Rect enemyRect = e->getCollisionRect();
        // 接触判定
        auto result = PlayerEnemyCollision::resolvePlayerEnemyCollision(
            playerRect, 
            prevFeet, 
            newFeet, 
            playerVv, 
            enemyRect
        );

        // 接触判定結果で処理を分ける
        if(result == PlayerEnemyCollisionResult::None){
            continue;
        }
        if(result == PlayerEnemyCollisionResult::StompEnemy){
            // 敵を踏みつけ(後で敵を消す)
            e->startDying();    // Dying状態へ遷移
            // プレイヤーはバウンドする
            player.setVerticalVelocity(-std::abs(PlayerConfig::JUMP_VELOCITY));
            // スコア加算
            ctrl.setScore(ctrl.getScore() + EnemyConfig::SCORE_AT_STOMP);
            continue;   // 同フレーム中に同一の敵を二度ふまないようにcountinue
        }
        if(result == PlayerEnemyCollisionResult::PlayerHit){
            ctrl.requestScene(GameScene::GameOver);
            return;
        }
    }
}

/**
 * @brief 落下死判定関数
 * 
 */
void CollisionSystem::checkFallDeath(SceneControl& ctrl){
    //double death_Y = GameConfig::WINDOW_HEIGHT + PlayerConfig::FRAME_H; // 画面外へ落下死たら終了
    double death_Y = world.WorldHeight + PlayerConfig::FRAME_H; // 規定したworldInfoを使う
    double playerBottom = player.getCollisionRect().y + player.getCollisionRect().h;
    if (playerBottom > death_Y){
        ctrl.requestScene(GameScene::GameOver);
        return;
    }
}

/**
 * @brief PlayingScene::onEnterで呼ばれ，内部で保持する変数などをクリアする関数
 * CollisionSystemでは現状何もしない
 * 
 */
void CollisionSystem::onStageLoaded(){

}
