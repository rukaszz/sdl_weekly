#include "BossAISystem.hpp"

#include <cmath>

#include "BossEnemy.hpp"
#include "BossConfig.hpp"
#include "Player.hpp"
#include "Block.hpp"
#include "WorldInfo.hpp"
#include "BossBattleState.hpp"

/**
 * @brief Construct a new Boss AI System:: Boss AI System object
 * 
 * @param boss_ 
 * @param player_ 
 * @param blocks_ 
 * @param blockRects_ 
 * @param world_ 
 * @param bossBattle_ 
 */
BossAISystem::BossAISystem(
    BossEnemy& boss_, 
    const Player& player_,
    const std::vector<Block>& blocks_,
    const std::vector<SDL_Rect>& blockRects_,
    const WorldInfo& world_,
    const BossBattleState& bossBattle_
) : boss(boss_)
  , player(player_)
  , blocks(blocks_)
  , blockRects(blockRects_)
  , world(world_)
  , bossBattle(bossBattle_)
{

}

/**
 * @brief ステージロード時の処理
 * 
 */
void BossAISystem::onStageLoaded(){
    // 今は何もしない
}

/**
 * @brief 更新関数
 * 
 * @param delta 
 */
void BossAISystem::update(double delta){
    // ボス戦が有効か
    if(!bossBattle.isActive()){
        return;
    }
    // ボスが生きているか
    if(!boss.isAlive()){
        return;
    }
    // 行動判断用センサー
    const EnemySensor sensor = buildBossSensor();
    boss.think(delta, sensor);
}

/**
 * @brief ボスの行動判断用のデータ収集
 * 
 * @return EnemySensor 
 */
EnemySensor BossAISystem::buildBossSensor() const{
    EnemySensor es{};
    // EnemySensorの各要素を収集
    const double bossCenter_X   = boss.getEntityCenter_X();
    const double bossCenter_Y   = boss.getEntityCenter_Y();
    const double playerCenter_X = player.getEntityCenter_X();
    const double playerCenter_Y = player.getEntityCenter_Y();
    // boss - player の距離
    const double dx = playerCenter_X - bossCenter_X;
    const double dy = playerCenter_Y - bossCenter_Y;
    // センサーの各要素へ格納
    es.dxToPlayer       = dx;
    es.dyToPlayer       = dy;
    es.distanceToPlayer = std::sqrt(dx*dx + dy*dy);
    es.playerOnLeft     = (playerCenter_X < bossCenter_X);
    es.playerBelow      = (playerCenter_Y > bossCenter_Y);
    es.playerInSight    = (std::abs(dx) < BossConfig::SIGHT_RANGE
                        && std::abs(dy) <= BossConfig::ATTACK_HEIGHT_TOLERANCE);
    // ボスの移動は今はしないので固定
    es.groundAhead      = true;
    es.wallAhead        = false;

    return es;
}
