#include "BossAISystem.hpp"

#include <cmath>

#include "BossEnemy.hpp"
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

}

