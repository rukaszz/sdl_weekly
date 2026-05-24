#include "BossBattleSystem.hpp"

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <cmath>

// オブジェクトなど
#include "BossEnemy.hpp"
#include "Block.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "MusicSystem.hpp"
#include "IGameEvents.hpp"
#include "GameEvent.hpp"

// 定数・ユーティリティなど
#include "BossConfig.hpp"
#include "DamageResult.hpp"
#include "StageDefinition.hpp"
#include "PlayerConfig.hpp"
#include "PlayerEnemyCollisionUtil.hpp"
#include "MusicId.hpp"
#include "UIConfig.hpp"

// ゲーム全体
#include "Camera.hpp"
#include "DrawBounds.hpp"
#include "Input.hpp"
#include "WorldInfo.hpp"

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
    // 呼び出し元で条件を指定しているが，ここでも守っておく
    // ボス戦がアクティブではない
    if(!state.isActive()){
        return;
    }
    // Dead(Dying完了済み)なら更新不要
    // Dying中はupdate()を呼んでアニメーションを進める必要があるため，ここではisDeadで判定する
    if(boss.isDead()){
        return;
    }
    boss.update(delta, is, bounds, blocks);
}

/**
 * @brief ボス戦の終了判定用関数
 * 
 * @param events 
 */
void BossBattleSystem::checkBattleResult(IGameEvents& events){
    // ボス戦が有効でなければ何もしない
    if(!state.isActive()){
        return;
    }
    // ボスが死んだ際の処理なので死んでいないなら何もしない
    if(!boss.isDead()){
        return;
    }
    // ボス戦を終了させる
    state.phase = BossBattlePhase::Defeated;
    // 演出
    events.addScore(BossConfig::SCORE_AT_DEATH);
    events.spawnParticle(
        ParticleEffectId::EnemyBurst, 
        boss.getEntityCenter_X(), 
        boss.getEntityCenter_Y()
    );
    events.startCameraShake(0.45, 12.0);
    // クリアシーンへ遷移
    events.requestScene(GameScene::Clear);
}

/**
 * @brief プレイヤーとボスの接触判定を行う
 * ※CollisionSystem::resolveEnemyCollision()も参考
 * 
 * @param player: プレイヤーの状態が変わる可能性があるので非const 
 * @param events 
 */
void BossBattleSystem::resolveBossPlayerCollision(Player& player, IGameEvents& events){
    // 状況の確認
    if(!state.isActive()){
        return;
    }
    if(!boss.isAlive()){
        return;
    }
    // 各種状態を変数へ格納する
    // プレイヤー
    SDL_Rect playerRect   = player.getCollisionRect();      // 当たり判定用矩形
    double playerPrevFeet = player.getPrevFeetCollision();  // 当たり判定用1フレーム前足元
    double playerNewFeet  = player.getFeetCollision();      // 当たり判定用足元
    double playerVv       = player.getVerticalVelocity();   // 垂直速度
    // ボス
    SDL_Rect bossRect = boss.getCollisionRect();            // 当たり判定用矩形

    // 接触状態をチェック(PlayerEnemyCollisionResultをもらう)
    auto result = PlayerEnemyCollision::resolvePlayerEnemyCollision(
        playerRect, playerPrevFeet, playerNewFeet, playerVv, bossRect
    );
    // 接触の状態でそれぞれ処理する
    // ボスを踏みつけた
    if(result == PlayerEnemyCollisionResult::StompEnemy){
        if(boss.canTakeDamage()){
            // ボスへダメージ
            boss.takeDamage(BossConfig::STOMP_DAMAGE);
            // プレイヤーはバウンド
            player.setVerticalVelocity(-std::abs(PlayerConfig::JUMP_VELOCITY));
            // 踏みつけ音
            events.playSound(SoundId::Stomp);
            // カメラシェイク
            events.startCameraShake(0.15, 8.0);
        } else {
            // 無敵中は弾くだけ(SE/シェイクはしない)
            player.setVerticalVelocity(-std::abs(PlayerConfig::JUMP_VELOCITY));
        }
        return;
    }
    // プレイヤーがボスに接触(接触ダメージを受ける)
    if(result == PlayerEnemyCollisionResult::PlayerHit){
        // プレイヤーはダメージを受けられるか
        DamageResult dr = player.tryTakeDamage();
        // デカ状態でのダメージ
        if(dr == DamageResult::Downgraded){
            // ダメージ音
            events.playSound(SoundId::Damage);
            // カメラシェイク
            events.startCameraShake(0.18, 10.0);
        }
        // チビ状態でのダメージ
        if(dr == DamageResult::Dead){
            // ダメージ音
            events.playSound(SoundId::Damage);
            // カメラシェイク
            events.startCameraShake(0.25, 14.0);
            // プレイヤー死亡イベント開始
            events.requestPlayerDeath(player.getEntityCenter_X(), player.getEntityCenter_Y());
        }
    }
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
