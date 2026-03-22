#include "ProjectileSystem.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

#include "FireBall.hpp"
#include "EnemyBullet.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "BossEnemy.hpp"

#include "GameUtil.hpp"
#include "WorldInfo.hpp"
#include "IGameEvents.hpp"

#include "EnemyConfig.hpp"
#include "BossConfig.hpp"
#include "FireBallConfig.hpp"
#include "EnemyBulletConfig.hpp"

#include "TurretEnemy.hpp"  // dynamic_cast対象

// ProjectileSystem.cpp内で閉じているヘルパ関数
namespace{
    /**
     * @brief 弾の種類を問わずに非活性/画面外へ出た弾を片付ける関数
     * cleanup()で呼ばれるヘルパ関数なので，static宣言する→実装もこのファイル内で完結させる
     * 
     * @tparam T 
     * @param vec 
     * @param world 
     * @param frame_W 
     * @param frame_H 
     */
    template<typename T> 
    void cleanupProjectiles(
        std::vector<std::unique_ptr<T>>& vec, 
        const WorldInfo& world, 
        int frame_W, 
        int frame_H
    )
    {
        // 条件を満たす要素を削除
        auto pred = [&](const std::unique_ptr<T>& p){
            // 非活性は削除対象
            if(!p->isActive()){
                return true;
            }
            // 画面外へ出た弾も削除対象
            const SDL_Rect r = p->getCollisionRect();
            return GameUtil::isOutOfWorldBounds(r, world.WorldWidth, world.WorldHeight, frame_W, frame_H);
        };
        auto it = std::remove_if(vec.begin(), vec.end(), pred);
        // remove_ifで削除対象を取り除いたイテレータを取得してeraseで消す
        vec.erase(it, vec.end());
    }
}

/**
 * @brief Construct a new Projectile System:: Projectile System object
 * 
 * @param fireballs_ 
 * @param enemyBullets_ 
 * @param blocks_ 
 * @param worldInfo_ 
 * @param fireballTexture_ 
 * @param enemyBulletTexture_ 
 */
ProjectileSystem::ProjectileSystem(
    std::vector<std::unique_ptr<FireBall>>& fireballs_,
    std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets_,
    const std::vector<Block>& blocks_,
    const WorldInfo& worldInfo_,
    Texture& fireballTexture_,
    Texture& enemyBulletTexture_
)
    : fireballs(fireballs_)
    , enemyBullets(enemyBullets_)
    , blocks(blocks_)
    , world(worldInfo_)
    , fireballTexture(fireballTexture_)
    , enemyBulletTexture(enemyBulletTexture_)
{

}

/**
 * @brief FireBall/EnemyBulletを更新する
 * 
 */
void ProjectileSystem::update(double delta){
    // クールダウンの減衰処理
    if(playerFireCooldownTimer > 0.0){
        playerFireCooldownTimer = std::max(0.0, playerFireCooldownTimer - delta);
    }
    // ファイアボール
    for(auto& f : fireballs){
        f->update(delta, blocks);
    }
    // 敵弾
    for(auto& eb : enemyBullets){
        eb->update(delta, blocks);
    }
}

/**
 * @brief ファイアボールの生成
 * 
 * @param x 
 * @param y 
 * @param dir 
 */
void ProjectileSystem::spawnPlayerFireball(double x, double y, Direction dir){
    fireballs.emplace_back(std::make_unique<FireBall>(x, y, dir, fireballTexture));
}

/**
 * @brief メンバ変数のfireballsでactive状態のものを数えて返す
 * 
 * @return int 
 */
int ProjectileSystem::countActivePlayerFireballs() const{
    int count = 0;
    for(const auto& f : fireballs){
        if(f->isActive()){
            ++count;
        }
    }
    return count;
}

/**
 * @brief プレイヤーがファイアボールを撃てるかをチェックする関数
 * 
 * @return true 
 * @return false 
 */
bool ProjectileSystem::canSpawnPlayerFireball() const{
    // クールダウン中は撃てない
    if(playerFireCooldownTimer > 0.0){
        return false;
    }
    // 規定の数が画面にあれば撃てない
    if(countActivePlayerFireballs() >= FireBallConfig::MAX_PLAYER_FIREBALLS){
        return false;
    }
    return true;
}

/**
 * @brief ファイアボールが画面に出現できるか(撃てるか)を判定してから撃つ
 * 
 * @param x 
 * @param y 
 * @param dir 
 * @return true 
 * @return false 
 */
bool ProjectileSystem::trySpawnPlayerFireball(double x, double y, Direction dir){
    // ファイアボール撃てるか判定
    if(!canSpawnPlayerFireball()){
        return false;
    }
    spawnPlayerFireball(x, y, dir);
    // クールダウン設定
    playerFireCooldownTimer = FireBallConfig::PLAYER_FIRE_COOLDOWN;
    return true;
}

/**
 * @brief 共通の弾発射関数
 * 
 * @param x 
 * @param y 
 * @param dir 
 */
void ProjectileSystem::spawnEnemyBullet(double x, double y, Direction dir){
    enemyBullets.emplace_back(
        std::make_unique<EnemyBullet>(x, y, dir, enemyBulletTexture)
    );
}

/**
 * @brief 敵弾の生成
 * 
 * @param enemies 
 */
void ProjectileSystem::spawnEnemyBulletsFromEnemies(std::vector<std::unique_ptr<Enemy>>& enemies){
    for(auto& e : enemies){
        // TurretEnemyをenemisから取得
        // dynamic_castはこの処理でのみ許容
        auto* turret = dynamic_cast<TurretEnemy*>(e.get());
        // TurretEnemyではないものは処理しない
        if(!turret){
            continue;
        }
        // fireRequestedがtrueなら発射できる→trueが返ってくる
        Direction dir;
        if(!turret->consumeFireRequest(dir)){
            continue;
        }
        // 発射できるならTurretEnemy中心座標から発射(座標は要調整)
        const double x = turret->getEntityCenter_X() + (dir == Direction::Right ? TurretConfig::MUZZLE_OFFSET_X : -TurretConfig::MUZZLE_OFFSET_X);
        const double y = turret->getEntityCenter_Y() + TurretConfig::MUZZLE_OFFSET_Y;
        enemyBullets.emplace_back(std::make_unique<EnemyBullet>(x, y, dir, enemyBulletTexture));
    }
}

/**
 * @brief 衝突解決関数を呼ぶ
 * 
 * @param player 
 * @param enemies 
 * @param ctrl 
 */
void ProjectileSystem::resolveCollisions(Player& player, std::vector<std::unique_ptr<Enemy>>& enemies, IGameEvents& events){
    // FireBall vs Enemy
    resolveFireballEnemyCollision(enemies, events);
    // EnemyBullet vs Player
    resolvePlayerEnemyBulletCollision(player, events);
}

/**
 * @brief FireBall vs Enemyの衝突解決
 * 
 * @param enemies 
 * @param ctrl 
 */
void ProjectileSystem::resolveFireballEnemyCollision(std::vector<std::unique_ptr<Enemy>>& enemies, IGameEvents& events){
    // ファイアボールを分解
    for(auto& f : fireballs){
        // 非活性のファイアボールは処理しない
        if(!f->isActive()){
            continue;
        }
        // ファイアボールの当たり判定用矩形取得
        const SDL_Rect fbr = f->getCollisionRect();
        // ファイアボールと敵の当たり判定を調べる
        for(auto& e : enemies){
            // 死亡状態の敵とは判定しない
            if(!e->isAlive()){
                continue;
            }
            // 敵の当たり判定用矩形取得
            const SDL_Rect er = e->getCollisionRect();
            // 非接触の場合は処理を抜ける
            if(!GameUtil::intersects(fbr, er)){
                continue;
            }
            // FireBall-Enemyが接触したら
            e->startDying();
            f->deactivate();
            // ctrl.setScore(ctrl.getScore() + EnemyConfig::SCORE_AT_FIREBALL);
            events.addScore(EnemyConfig::SCORE_AT_FIREBALL);
            break;  // ヒットしたら終了(ファイアボールは貫通しない)
        }
    }
}

/**
 * @brief EnemyBullet vs Playerの衝突解決
 * 
 * @param player 
 * @param ctrl 
 */
void ProjectileSystem::resolvePlayerEnemyBulletCollision(Player& player, IGameEvents& events){
    // Playerの当たり判定用矩形取得
    const SDL_Rect pr = player.getCollisionRect();
    // 個々のEnemyBulletと判定
    for(auto& eb : enemyBullets){
        if(!eb->isActive()){
            continue;
        }
        // 各EnemyBulletの当たり判定用矩形取得
        const SDL_Rect ebr = eb->getCollisionRect();
        // 衝突していないなら次へ
        if(!GameUtil::intersects(ebr, pr)){
            continue;
        }
        // 衝突した
        // 弾は非活性に※無的中でも非活性
        eb->deactivate();
        if(player.tryTakeDamage()){
            // playerに当たったのでGameOverに
            events.requestScene(GameScene::GameOver);
        }
        // 処理は抜ける
        break;
    }
}

/**
 * @brief 各弾の状態を確認し不要な片付ける関数
 * 画面外へ出ている/非活性になっているなど
 * 
 */
void ProjectileSystem::cleanup(){
    // ファイアボール片付け
    // cleanupFireballs();
    cleanupProjectiles(fireballs, world, FireBallConfig::FRAME_W, FireBallConfig::FRAME_H);
    // 敵弾片付け
    // cleanupEnemyBullets();
    cleanupProjectiles(enemyBullets, world, EnemyBulletConfig::FRAME_W, EnemyBulletConfig::FRAME_H);
}

/**
 * @brief PlayingScene::onEnterで呼ばれ，内部で保持する変数などをクリアする関数
 * 
 */
void ProjectileSystem::onStageLoaded(){
    playerFireCooldownTimer = 0.0;
}

/**
 * @brief 
 * 
 * @param boss 
 * @param events 
 */
void ProjectileSystem::resolveFireballBossCollision(BossEnemy& boss, IGameEvents& events){
    // 撃破済みなら判定しない
    if(boss.isDefeated()){
        return;
    }
    // ボスの当たり判定矩形を取得
    const SDL_Rect bossRect = boss.getCollisionRect();
    for(auto& f : fireballs){
        // 非活性は処理しない
        if(f->isActive()){
            continue;
        }
        // 衝突していないならスキップ
        if(!GameUtil::intersects(f->getCollisionRect(), bossRect)){
            continue;
        }
        // ヒット：ファイアボール非活性化→ボスへダメージ
        f->deactivate();
        boss.takeDamage(BossConfig::FIREBALL_DAMAGE);
        // スコア加算(するなら)
        // if(boss.isDefeated()){
        //     events.addScore(BossConfig::SCORE_AT_DEATH);
        // }
        break;  // 1フレームに1ヒットする
    }
}

/**
 * @brief ボスの弾発射処理
 * 
 * @param boss 
 */
void ProjectileSystem::spawnBossBullets(BossEnemy& boss){
    // 方向決定
    Direction dir;
    if(!boss.consumeFireRequest(dir)){
        return;
    }
    // ボス中心から発射
    const double x = boss.getEntityCenter_X();
    const double y = boss.getEntityCenter_Y();
    enemyBullets.emplace_back(
        std::make_unique<EnemyBullet>(x, y, dir, enemyBulletTexture)
    );
}
