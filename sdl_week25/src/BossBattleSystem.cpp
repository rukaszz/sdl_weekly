#include "BossBattleSystem.hpp"

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

#include "MusicId.hpp"
#include "UIConfig.hpp"

#include "Player.hpp"
#include "BossEnemy.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "MusicSystem.hpp"
#include "IGameEvents.hpp"
#include "Block.hpp"
#include "WorldInfo.hpp"
#include "StageDefinition.hpp"
#include "Input.hpp"
#include "DrawBounds.hpp"
#include "Camera.hpp"

/**
 * @brief Construct a new Boss Battle System:: Boss Battle System object
 * 
 */
BossBattleSystem::BossBattleSystem(
    BossEnemy& boss_,
    const Player& player_,
    const std::vector<Block>& blocks_,
    const std::vector<SDL_Rect>& blockRects_,
    const WorldInfo& world_
) : boss(boss_)
  , bossAI(boss_, player_, blocks_, blockRects_, world_, state)
  , player(player_)
  , blocks(blocks_)
{

}

/**
 * @brief ボス戦に関するステージ定義を取得する
 * PlayingScene由来
 * @param def 
 */
void BossBattleSystem::init(const StageDefinition& def){
    // BossBattleState
    state.phase          = def.bossBattleDef.enabled
        ? BossBattlePhase::WaitingTrigger : BossBattlePhase::None;
    state.trigger_X      = def.bossBattleDef.trigger_X;
    state.cameraMin_X    = def.bossBattleDef.cameraMin_X;
    state.cameraMax_X    = def.bossBattleDef.cameraMax_X;
    // ボスのスポーン位置などはloadStage()でやる
}

/**
 * @brief 現状は何もしない
 * ※ボスの行動に関係する処理の初期化用
 * BossAISystem由来
 */
void BossBattleSystem::onStageLoaded(){
    // 今は何もしない
}

/**
 * @brief ボス戦開始地点を通過したかの判定用トリガー
 * PlayingScene由来
 * 
 * @param music 
 * @param events 
 */
void BossBattleSystem::updateTrigger(MusicSystem& music, IGameEvents& events){
    // ボス戦が無いステージでは処理しない
    if(!state.isWaiting()){
        return;
    }
    // プレイヤーの中心座標
    const double player_X = player.getEntityCenter_X();
    if(player_X >= state.trigger_X){
        state.phase = BossBattlePhase::Active;
        // BGM再生
        music.playIfChanged(MusicId::Boss);
        // 画面シェイク
        events.startCameraShake(0.30, 12.0);
    }    
}

/**
 * @brief ボスのAI更新用
 * 
 * @param delta 
 */
void BossBattleSystem::updateAI(double delta){
    // ボス戦が有効か
    if(!state.isActive()){
        return;
    }
    // ボスが生きているか
    if(!boss.isAlive()){
        return;
    }
    // 行動判断用センサー
    const EnemySensor sensor = bossAI.buildBossSensor();
    boss.think(delta, sensor);
}

/**
 * @brief ボスのupdate呼び出し用のラッパ
 * 
 * @param delta 
 * @param bounds 
 * @param is 
 */
void BossBattleSystem::updateBoss(double delta, const DrawBounds& bounds, const InputState& is){
    boss.update(delta, is, bounds, blocks);
}

/**
 * @brief ボス戦の終了判定用関数
 * 
 * @param events 
 */
void BossBattleSystem::checkBattleResult(IGameEvents& events){
    if(!state.isActive()){
        return;
    }
    // ボスが死んだ際の処理
    if(!boss.isDead()){
        return;
    }
    state.phase = BossBattlePhase::Defeated;
    events.requestScene(GameScene::Clear);
}

/**
 * @brief ボスのHPバー描画
 * 
 * @param renderer 
 * @param camera 
 */
void BossBattleSystem::renderHpBar(Renderer& renderer, const Camera& camera) const{
    // 座標
    const int bar_W = UIConfig::BossHpBarConfig::BAR_W;
    const int bar_H = UIConfig::BossHpBarConfig::BAR_H;
    const int bar_X = static_cast<int>((camera.width - bar_W)/2.0); // 画面中央
    const int bar_Y = UIConfig::BossHpBarConfig::BAR_Y;
    // ボスのHP取得
    const int hp = boss.getHp();
    const int maxHp = boss.getMaxHp();
    if(maxHp <= 0){
        return;
    }
    // バーの比率
    double hpRatio = std::clamp(static_cast<double>(hp) / static_cast<double>(maxHp), 0.0, 1.0);
    // HPバーの幅を比率に合わせる
    const int filled_W = static_cast<int>(bar_W * hpRatio);
    // HPバーの矩形定義(Frameは少し大き目)
    const int offset = UIConfig::BossHpBarConfig::FRAME_OFFSET;
    const SDL_Rect frameRect = {bar_X-offset, bar_Y-offset, bar_W+(offset*2), bar_H+(offset*2)};
    const SDL_Rect bgRect    = {bar_X, bar_Y, bar_W, bar_H};
    const SDL_Rect fillRect  = {bar_X, bar_Y, filled_W, bar_H};
    // HPバーの矩形色定義
    // 枠
    const SDL_Color frameColor = UIConfig::BossHpBarConfig::FRAME_COLOR;
    // 背景
    const SDL_Color bgColor    = UIConfig::BossHpBarConfig::BG_COLOR;
    // HPバーの色
    SDL_Color fillColor{};
    if(hpRatio > 0.6){
        // 緑
        fillColor = UIConfig::BossHpBarConfig::HIGH_HP_COLOR;
    } else if(hpRatio > 0.3){
        // 黄
        fillColor = UIConfig::BossHpBarConfig::MIDDLE_HP_COLOR;
    } else {
        // 赤
        fillColor = UIConfig::BossHpBarConfig::LOW_HP_COLOR;
    }
    // 描画：枠→背景→HPバーの順
    // 枠
    renderer.drawRect(frameRect, frameColor);
    // 背景
    renderer.drawRect(bgRect, bgColor);
    // HPバー
    renderer.drawRect(fillRect, fillColor);
}
