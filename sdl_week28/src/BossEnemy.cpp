#include "BossEnemy.hpp"

#include <algorithm>
#include <cmath>

#include "Block.hpp"
#include "BossConfig.hpp"
#include "DrawBounds.hpp"
#include "Direction.hpp"
#include "EnemySensor.hpp"
#include "Input.hpp"

/**
 * @brief Construct a new Boss Enemy:: Boss Enemy object
 * 
 * @param tex 
 */
BossEnemy::BossEnemy(Texture& tex)
    : Enemy(
        tex,
        BossConfig::MOVE_SPEED,
        BossConfig::FRAME_W,
        BossConfig::FRAME_H,
        BossConfig::NUM_FRAMES,
        BossConfig::ANIM_INTERVAL
    )
{
    sprite.setFrame(0);
}

/**
 * @brief ボスの行動判断用関数
 * 弾を撃つなどの判断をする
 * 
 * @param delta 
 * @param es 
 */
void BossEnemy::think(double delta, const EnemySensor& es){
    // 生きていない場合は何もしない
    if(!isAlive()){
        return;
    }
    // クールダウンの減衰
    bossFireCooldownTimer = std::max(0.0, bossFireCooldownTimer - delta);
    bossJumpCooldownTimer = std::max(0.0, bossJumpCooldownTimer - delta);
    // プレイヤーのいる方向を向く
    dir = es.playerOnLeft ? Direction::Left : Direction::Right;
    // 無敵時間中ずっと踏まれないように逃げる
    if(invincibleTimer > 0.0){
        // プレイヤー左→右(x増加方向)へ，プレイヤー右→左(x減少方向)へ
        hv = es.playerOnLeft ? speed : -speed;
        // ジャンプ可能ならジャンプをして逃げる
        if(isOnGround()){
            setVerticalVelocity(-BossConfig::JUMP_VELOCITY);
        }
        // 逃げているときは攻撃しない
        return;
    }
    // 追跡範囲外では動かない
    if(std::abs(es.dxToPlayer) > BossConfig::CHASE_RANGE){
        hv = 0.0;
        return;
    }
    // ここに到達=プレイヤー視界内
    hv = es.playerOnLeft ? -speed : speed;
    // 接地中 かつ クールダウン終了 でジャンプ
    if(isOnGround() && bossJumpCooldownTimer <= 0.0){
        // 垂直速度設定(SDLは上が原点なのでマイナス)
        setVerticalVelocity(-BossConfig::JUMP_VELOCITY);
        // ジャンプクールダウン設定
        bossJumpCooldownTimer = BossConfig::JUMP_INTERVAL;
    }
    // プレイヤーが視界内 かつ クールダウン終了 で攻撃
    if(es.playerInSight && !fireRequested && bossFireCooldownTimer <= 0.0){
        // プレイヤーの左右の向きで発射方向を決める
        fireDir = es.playerOnLeft ? Direction::Left : Direction::Right;
        fireRequested = true;
        bossFireCooldownTimer = BossConfig::FIRE_INTERVAL;
    }
    
}

/**
 * @brief ボスの更新関数
 * 
 * @param delta 
 * @param is 
 * @param b 
 * @param blocks 
 */
void BossEnemy::update(double delta, const InputState& is, DrawBounds b, const std::vector<Block>& blocks){
    // 無敵時間中なら無敵時間減衰
    if(invincibleTimer > 0.0){
        invincibleTimer = std::max(0.0, invincibleTimer - delta);
    }
    Enemy::update(delta, is, b, blocks);
}

/**
 * @brief ステージロード時に呼び出してデータをセットする
 * 
 * @param hp_ 
 * @param spawn_X 
 * @param spawn_Y 
 */
void BossEnemy::reset(int initHp, double spawn_X, double spawn_Y){
    // HP設定
    hp = initHp;
    maxHp = initHp;
    // 出現位置(stageDefinitionより)
    setPosition(spawn_X, spawn_Y);
    // 各種変数初期化
    invincibleTimer = 0.0;
    bossFireCooldownTimer = 0.0;
    bossJumpCooldownTimer = 0.0;
    fireRequested =false;
    fireDir = Direction::Left;
    // stateやhvなど共通項目のリセット
    resetEnemyStateForSpawn();
}

/**
 * @brief ボスのHPを減らす関数
 * 
 * @param damage 
 */
void BossEnemy::takeDamage(int damage){
    // 敗北しているなら何もしない
    if(!isAlive()){
        return;
    }
    // 無敵時間なら減らさない
    if(!canTakeDamage()){
        return;
    }
    // 被弾処理
    hp -= damage;
    invincibleTimer = BossConfig::DAMAGE_INVINCIBLE_TIME;
    // hpが0以下になったら敗北処理
    if(hp <= 0){
        hp = 0;
        // 弾の発射停止
        fireRequested = false;
        // 移動も止める
        hv = 0.0;
        startDying();
    }
}

/**
 * @brief 発射要求に応える関数
 * ※PlayingSceneなどがfireRequestedを触らないようにするため
 * 
 * @param outDir 
 * @return true 
 * @return false 
 */
bool BossEnemy::consumeFireRequest(Direction& outDir) noexcept{
    if(!fireRequested){
        return false;
    }
    outDir = fireDir;
    fireRequested = false;
    return true;
}

/**
 * @brief 被ダメージ時の無敵時間中の点滅表現用関数
 * 
 * @return true 
 * @return false 
 */
bool BossEnemy::shouldRender() const{
    // 無敵時間中しか処理しない=trueを返す
    if(!(invincibleTimer > 0.0)){
        return true;
    }
    // delta時間で減少するinvincibleTimerで点滅用にtrue/false切り替え
    const int phase = static_cast<int>(invincibleTimer * 20.0);
    return (phase % 2) == 0;
}

/**
 * @brief Characterのdrawをオーバライドしている
 * 点滅用にCharacter::draw()の呼び出しを切り替える
 * 
 * @param renderer 
 * @param camera 
 */
void BossEnemy::draw(Renderer& renderer, const Camera& camera){
    if(!shouldRender()){
        return;
    }
    Enemy::draw(renderer, camera);
}
