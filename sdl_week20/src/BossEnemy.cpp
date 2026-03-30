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
 * @brief TurretEnemyの判断用関数
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
    // 動かない(砲台)
    hv = 0.0;
    // 発射待機の時間を減らす
    bossFireCooldownTimer = std::max(0.0, bossFireCooldownTimer - delta);

    // 条件群
    // プレイヤーが視界内にいるときだけ撃つ
    if(!es.playerInSight){
        return;
    }
    // 発射要求があるか
    if(fireRequested){
        return;
    }
    // クールタイムではないか
    if(bossFireCooldownTimer > 0.0){
        return;
    }
    // プレイヤーの左右の向きで発射方向を決める
    fireDir = es.playerOnLeft ? Direction::Left : Direction::Right;
    fireRequested = true;
    bossFireCooldownTimer = BossConfig::FIRE_INTERVAL;
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
    hp = initHp;
    maxHp = initHp;
    setPosition(spawn_X, spawn_Y);
    invincibleTimer = 0.0;
    bossFireCooldownTimer = 0.0;
    fireRequested =false;
    fireDir = Direction::Left;
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
void BossEnemy::draw(Renderer& renderer, Camera& camera){
    if(!shouldRender()){
        return;
    }
    Enemy::draw(renderer, camera);
}
