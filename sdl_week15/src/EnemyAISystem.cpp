#include "EnemyAISystem.hpp"

#include <vector>
#include <memory>
#include <cassert>

#include <SDL2/SDL.h>   // TODO：SDLへの依存は後で取り除く(Rectを置き換える)

#include "Player.hpp"
#include "Enemy.hpp"
#include "Block.hpp"
#include "WorldInfo.hpp"

#include "GameUtil.hpp"
#include "EnemySensor.hpp"
#include "EnemyAIConfig.hpp"

/**
 * @brief Construct a new Enemy A I System:: Enemy A I System object
 * 
 * @param enemies_ 
 * @param blocks_ 
 * @param world_ 
 * @param player_ 
 */
EnemyAISystem::EnemyAISystem(
    std::vector<std::unique_ptr<Enemy>>& enemies_, 
    const std::vector<Block>& blocks_, 
    const std::vector<SDL_Rect>& blockRects_, 
    const WorldInfo& world_, 
    const Player& player_
)
    : enemies(enemies_)
    , blocks(blocks_)
    , blockRects(blockRects_)
    , world(world_)
    , player(player_)
{

}

/**
 * @brief Enemyの判断に使用するSensorの収集などを実施する関数
 * 
 * @param delta 
 */
void EnemyAISystem::update(double delta){
    // センサーのクリア
    sensors.clear();
    if(sensors.capacity() < enemies.size()){
        sensors.reserve(enemies.size());
    }
    // センサーのデータ収集
    gatherEnemySensors();
    // Enemyの行動決定
    runEnemyAI(delta);
}

/**
 * @brief Enemyの行動を決定するための情報を収集する
 * ゲーム全体から情報を収集する
 * EnemyはoutEnemySensorの結果を基に行動を決定する→Gameと直接依存しない
 * 
 * @param outSensors：収集した結果(出力) 
 */
void EnemyAISystem::gatherEnemySensors(){
    // 出力用センサの初期化
    // outEnemySensors.clear();
    // outEnemySensors.reserve(enemies.size());

    // ゲームの情報
    EnemySensorContext esc{
        .blocks = blocks, 
        .blockRects = blockRects,
        .worldWidth = world.WorldWidth, 
        .worldHeight = world.WorldHeight, 
        .playerInfo = PlayerInfo{
            player.getEntityCenter_X(), 
            player.getEntityCenter_Y()
        }
    };

    // 敵ごとに情報処理
    for(const auto& ePtr : enemies){
        // Enemy個別の参照を取得(vectorはポインタで持っているため)
        Enemy& enemy = *ePtr;
        EnemySensor enemySensor = buildEnemySensor(enemy, esc);
        // outEnemySensors.push_back(enemySensor);
        sensors.push_back(enemySensor);
    }
}

/**
 * @brief ゲームの情報からEnemyの行動決定に必要な処理を実施し結果を返却する
 * 各種処理を内部で呼び出す
 * 
 * 
 * @param enemy 
 * @param esc 
 * @return EnemySensor 
 */
EnemySensor EnemyAISystem::buildEnemySensor(const Enemy& enemy, const EnemySensorContext& esc) const{
    EnemySensor es{};
    // プレイヤー情報から関係性を整理
    fillPlayerRelation(enemy, esc, es);
    // 床に対する行動
    fillGroundAhead(enemy, esc, es);
    // 壁(水平方向にあるブロック)に対する行動
    fillWallAhead(enemy, esc, es);
    return es;
}

/**
 * @brief プレイヤー情報を確認し，Player-Enemy間の距離などを調べる
 * 
 * @param enemy：enemiesの各ポインタの参照 
 * @param esc：Gameの情報 
 * @param outSensor：処理結果を持つ構造体 
 */
void EnemyAISystem::fillPlayerRelation(const Enemy& enemy, 
                                       const EnemySensorContext& esc, 
                                       EnemySensor& outSensor) const
{
    // 調査対象の敵の中心座標
    const double enemyCenter_X = enemy.getEntityCenter_X();
    const double enemyCenter_Y = enemy.getEntityCenter_Y();
    // Player-Enemy間の距離の差分
    const double dx = esc.playerInfo.center_X - enemyCenter_X;
    const double dy = esc.playerInfo.center_Y - enemyCenter_Y;
    // 差分格納
    outSensor.dxToPlayer = dx;
    outSensor.dyToPlayer = dy;
    // ベクトル(2点間)の距離
    outSensor.distanceToPlayer = std::sqrt(dx*dx + dy*dy);
    // Playerが左側にいるか(左のほうがxが小さい)
    outSensor.playerOnLeft     = (esc.playerInfo.center_X < enemyCenter_X);
    // Playerが下にいるか(上のほうがyが小さい)
    outSensor.playerBelow      = (esc.playerInfo.center_Y > enemyCenter_Y);
    // プレイヤーが視界内にいるか
    // 簡単に両者のx/y軸の差分でチェックする
    outSensor.playerInSight = (std::abs(dx) <= EnemyAIConfig::SIGHT_MAX_X &&
                               std::abs(dy) <= EnemyAIConfig::SIGHT_MAX_Y);
}

/**
 * @brief Enemyの床に関する処理
 * 次の移動先が崖か(落下するか)を調べる
 * 
 * @param enemy 
 * @param esc 
 * @param outSensor groundProbe
 */
void EnemyAISystem::fillGroundAhead(const Enemy& enemy, 
                                    const EnemySensorContext& esc, 
                                          EnemySensor& outSensor) const
{
    // Enemyの情報取得
    // 当たり判定用矩形
    SDL_Rect er = enemy.getCollisionRect();
    // 進む方向(向いている方向)
    const Direction dir = enemy.getDirection();
    // 右を向いているか
    const bool facingRight = (dir == Direction::Right);

    // groundAheadの決定：一歩先に床があるか
    SDL_Rect groundProbe{};
    bool hasGround = false;
    // 右向き・左向きで判定用xを変える
    groundProbe.x = facingRight ? (er.x + er.w) : (er.x - EnemyAIConfig::PROBE_WIDTH);
    // x以外の判定全体
    groundProbe.y = er.y + er.h;    // 足元のちょっと下
    groundProbe.w = EnemyAIConfig::PROBE_WIDTH;
    groundProbe.h = EnemyAIConfig::GROUND_PROBE_DEPTH;
    // ブロックとの接触をAABBで判定し，接触していれば崖ではない→進めると判断
    // block→BlockType用，blockRect→当たり判定用
    for(std::size_t i = 0; i < esc.blocks.size(); ++i){
        // blocks取得
        const auto& b = esc.blocks[i];
        // 通常の床/すり抜け床以外は判定しない
        // この条件はPhysics::resolveBlockCollisionXXXの処理に合わせること
        if(b.type != BlockType::Standable && b.type != BlockType::DropThrough){
            continue;
        }
        // blocksの矩形取得
        const SDL_Rect& br = esc.blockRects[i];
        // Enemyの少し先の座標とブロックで接触判定
        if(GameUtil::intersects(groundProbe, br)){
            hasGround = true;
            break;
        }
    }
    // world端処理(端も崖とみなして引き返す)
    if(facingRight){
        // 右に向いているときにProbeがworldの外に出そうか
        if(groundProbe.x + groundProbe.w >= static_cast<int>(esc.worldWidth)){
            hasGround = false;
        }
    } else {
        // 左を向いているときにProbeがworldの外に出そうか
        if(groundProbe.x <= 0){
            hasGround = false;
        }
    }
    // 最終的な結果返却
    // hasGround = true;
    outSensor.groundAhead = hasGround;
}

/*
void EnemyAISystem::fillGroundAhead(const Enemy& enemy, 
                                    const EnemySensorContext& esc, 
                                          EnemySensor& outSensor) const
{
    // Enemyの情報取得
    // 当たり判定用矩形
    SDL_Rect er = enemy.getCollisionRect();
    // 進む方向(向いている方向)
    const Direction dir = enemy.getDirection();
    // 右を向いているか
    const bool facingRight = (dir == Direction::Right);

    // groundAheadの決定：一歩先に床があるか
    SDL_Rect groundProbe{};
    bool hasGround = false;
    // 右向き・左向きで判定用xを変える
    groundProbe.x = facingRight ? (er.x + er.w) : (er.x - EnemyAIConfig::PROBE_WIDTH);
    // x以外の判定全体
    groundProbe.y = er.y + er.h;    // 足元のちょっと下
    groundProbe.w = EnemyAIConfig::PROBE_WIDTH;
    groundProbe.h = EnemyAIConfig::GROUND_PROBE_DEPTH;
    // ブロックとの接触をAABBで判定し，接触していれば崖ではない→進めると判断
    for(const auto& b : esc.blocks){
        // 通常の床/すり抜け床以外は判定しない
        // この条件はPhysics::resolveBlockCollisionXXXの処理に合わせること
        if(b.type != BlockType::Standable && b.type != BlockType::DropThrough){
            continue;
        }
        SDL_Rect br = GameUtil::blockToRect(b); // TODO：コストが高いのであとでキャッシュにする
        // Enemyの少し先の座標とブロックで接触判定
        if(GameUtil::intersects(groundProbe, br)){
            hasGround = true;
            break;
        }
    }
    // world端処理(端も崖とみなして引き返す)
    if(facingRight){
        // 右に向いているときにProbeがworldの外に出そうか
        if(groundProbe.x + groundProbe.w >= static_cast<int>(esc.worldWidth)){
            hasGround = false;
        }
    } else {
        // 左を向いているときにProbeがworldの外に出そうか
        if(groundProbe.x <= 0){
            hasGround = false;
        }
    }
    // 最終的な結果返却
    // hasGround = true;
    outSensor.groundAhead = hasGround;
}
*/

/**
 * @brief Enemyの壁に関する処理
 * 次の移動先が壁か(ブロックにめり込むか)を調べる
 * 
 * @param enemy 
 * @param esc 
 * @param outSensor 
 */
void EnemyAISystem::fillWallAhead(const Enemy& enemy, 
                                  const EnemySensorContext& esc, 
                                        EnemySensor& outSensor) const
{
    // Enemyの情報取得
    // 当たり判定用矩形
    SDL_Rect er = enemy.getCollisionRect();
    // 進む方向(向いている方向)
    const Direction dir = enemy.getDirection();
    // 右を向いているか
    const bool facingRight = (dir == Direction::Right);

    // wallAheadの決定：一歩先に壁があるか
    SDL_Rect wallProbe{};
    bool hasWall = false;
    // 調べる方向の決定
    if (facingRight) {
        wallProbe.x = er.x + er.w;  // 右方向チェック
    } else {
        wallProbe.x = er.x - EnemyAIConfig::WALL_PROBE_DEPTH;  // 左方向チェック
    }
    // 体全体の情報
    wallProbe.y = er.y;
    wallProbe.w = EnemyAIConfig::WALL_PROBE_DEPTH;
    wallProbe.h = er.h;
    // 壁(ブロックとのチェック)
    for(std::size_t i = 0; i < esc.blocks.size(); ++i){
        // ブロック取得
        const auto& b = esc.blocks[i];
        // 通常の床以外は判定しない
        if(b.type != BlockType::Standable){
            continue;
        }
        const auto& br = esc.blockRects[i];
        // Enemyのちょっと先の座標とブロックで接触判定
        if(GameUtil::intersects(wallProbe, br)){
            hasWall = true;
            break;
        }
    }
    // worldの端も壁扱い
    if(facingRight){
        // 右に向いているときにProbeがworldの外に出そうか
        if(wallProbe.x + wallProbe.w >= static_cast<int>(esc.worldWidth)){
            hasWall = true;
        }
    }else{
        // 左を向いているときにProbeがworldの外に出そうか
        if(wallProbe.x <= 0){
            hasWall = true;
        }
    }
    // 最終的な結果返却
    outSensor.wallAhead = hasWall;
}

/*
void EnemyAISystem::fillWallAhead(const Enemy& enemy, 
                                  const EnemySensorContext& esc, 
                                        EnemySensor& outSensor) const
{
    // Enemyの情報取得
    // 当たり判定用矩形
    SDL_Rect er = enemy.getCollisionRect();
    // 進む方向(向いている方向)
    const Direction dir = enemy.getDirection();
    // 右を向いているか
    const bool facingRight = (dir == Direction::Right);

    // wallAheadの決定：一歩先に壁があるか
    SDL_Rect wallProbe{};
    bool hasWall = false;
    // 調べる方向の決定
    if (facingRight) {
        wallProbe.x = er.x + er.w;  // 右方向チェック
    } else {
        wallProbe.x = er.x - EnemyAIConfig::WALL_PROBE_DEPTH;  // 左方向チェック
    }
    // 体全体の情報
    wallProbe.y = er.y;
    wallProbe.w = EnemyAIConfig::WALL_PROBE_DEPTH;
    wallProbe.h = er.h;
    // 壁(ブロックとのチェック)
    for(const auto& b : esc.blocks){
        // 通常の床以外は判定しない
        if(b.type != BlockType::Standable){
            continue;
        }
        SDL_Rect br = GameUtil::blockToRect(b); // TODO：コストが高いのであとでキャッシュにする
        // Enemyのちょっと先の座標とブロックで接触判定
        if(GameUtil::intersects(wallProbe, br)){
            hasWall = true;
            break;
        }
    }
    // worldの端も壁扱い
    if(facingRight){
        // 右に向いているときにProbeがworldの外に出そうか
        if(wallProbe.x + wallProbe.w >= static_cast<int>(esc.worldWidth)){
            hasWall = true;
        }
    }else{
        // 左を向いているときにProbeがworldの外に出そうか
        if(wallProbe.x <= 0){
            hasWall = true;
        }
    }
    // 最終的な結果返却
    outSensor.wallAhead = hasWall;
}
*/

/**
 * @brief EnemyにSensorの結果を渡して行動を決定させる
 * 
 * @param delta 
 * @param sensors 
 */
void EnemyAISystem::runEnemyAI(double delta){
    // 敵とセンサの要素数をチェック(gatherが正しく動作する限り同サイズになる)
    // gatherする対象をフィルタするならenemiesのインデックスにsensorsが一致しなければならない
    assert(enemies.size() == sensors.size());
    // enemiesのそれぞれにSensorを渡す
    for(std::size_t i = 0; i < enemies.size(); ++i){
        enemies[i]->think(delta, sensors[i]);
    }
}

/**
 * @brief ステージロード時にセンサーを初期化する
 * 
 */
void EnemyAISystem::onStageLoaded(){
    sensors.clear();
    // 必要になればshrink_to_fit()を実施する
    // sensors.shrink_to_fit();    // enemiesは倒されると減るので領域を数に合わせる
    sensorAccum = 0.0;
}
