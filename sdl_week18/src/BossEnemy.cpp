#include "BossEnemy.hpp"

#include <algorithm>

#include "Block.hpp"
#include "BossConfig.hpp"
#include "DrawBounds.hpp"
#include "Direction.hpp"
#include "EnemySensor.hpp"
#include "Enemy.hpp"
#include "Input.hpp"
#include "StageDefinition.hpp"
#include "Sprite.hpp"

/**
 * @brief Construct a new Boss Enemy:: Boss Enemy object
 * 
 * @param tex 
 */
BossEnemy::BossEnemy(Texture& tex)
    : Enemy(
        tex,
        BossConfig::RUN_MAX_SPEED,
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
    // 動かない(砲台)
    hv = 0.0;
    // 発射の時間を減らす
    bossFireCooldownTimer -= delta;

    // 条件：プレイヤーが視界内にいるときだけ撃つ
    if(!es.playerInSight ||
        es.distanceToPlayer > 600.0 ||      // プレイヤーとの距離が一定以上
        std::abs(es.dyToPlayer) > 64.0){    // プレイヤーの高さが離れている(dyが一定以上)
        return; // 撃たない
    }
    // 条件を満たしてもすでにtrueなら何もしない→連射抑制
    if(fireRequested){
        return;
    }
    // プレイヤーが視界内にいたら
    if(bossFireCooldownTimer <= 0.0){
        // プレイヤーの左右の向きで発射方向を決める
        fireDir = es.playerOnLeft ? Direction::Left : Direction::Right;
        fireRequested = true;
        bossFireCooldownTimer = 1.5;
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
    (void)is;
    // 敗北しているか
    if(defeated){
        // Dyingなどの演出で回す用
        Enemy::update(delta, is, b, blocks);
        return;
    }
    // 無敵時間中なら無敵時間減衰
    if(damageInvincibleTimer > 0.0){
        damageInvincibleTimer = std::max(0.0, damageInvincibleTimer - delta);
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
    setPosition(spawn_X, spawn_Y);
    defeated = false;
    damageInvincibleTimer = 0.0;
    bossFireCooldownTimer = 0.0;
    fireRequested =false;
    fireDir = Direction::Left;
    hv = 0.0;
    vv = 0.0;
}

/**
 * @brief ボスのHPを減らす関数
 * 
 * @param damage 
 */
void BossEnemy::takeDamage(int damage){
    // 敗北しているなら何もしない
    if(defeated){
        return;
    }
    // 無敵時間なら減らさない
    if(damageInvincibleTimer > 0.0){
        return;
    }
    // 被弾処理
    hp -= damage;
    damageInvincibleTimer = 1.0;
    // hpが0以下になったら敗北処理
    if(hp <= 0){
        hp = 0;
        defeated = true;
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
