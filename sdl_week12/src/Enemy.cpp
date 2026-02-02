// 定数
#include "GameConfig.hpp"
#include "EnemyConfig.hpp"

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Enemy.hpp"
#include "Input.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

/**
 * Enemyクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 */

/**
 * @brief Construct a new Enemy:: Enemy object
 * 
 * @param tex 
 */
Enemy::Enemy(Texture& tex)
    : Character(
        0, 0,                               // 初期座標
        100.0,                              // speed
        0.0,                                // 水平速度
        0.0,                                // 垂直速度
        Direction::Right,                   // dir
        tex,                                // texture
        EnemyConfig::FRAME_W, 
        EnemyConfig::FRAME_H, 
        EnemyConfig::NUM_FRAMES,            // maxFrames
        0.1                                 // animInterval
    )
    
{
    state = EnemyState::Alive;
    sprite.setFrame(0);
}

/**
 * @brief Enemyの動きを記述する(※現状は左右に動くだけ)
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * 
 * @param delta: 差分 
 * @param (ImputState) プレイヤーの入力(現状は無名) 
 * @param drawableSize: 描画可能範囲 
 * @param (const std::vector<Block>&) ブロックの判定(現状は無名)
 */
void Enemy::update(double delta, const InputState& , DrawBounds bounds, const std::vector<Block>& ){
    double leftBound = 0.0;
    double rightBound = bounds.drawableWidth - static_cast<double>(sprite.getDrawWidth());

    switch(state){
    case EnemyState::Dead:
        // 死亡状態は無視する    
        return; 
    case EnemyState::Dying:
        // その場にとどまって，一定時間経過で死ぬ
        dyingTime += delta;
        // 一定時間経過でDying→Deadへ
        if(dyingTime >= EnemyConfig::DYING_DURATION){
            state = EnemyState::Dead;
        }
        return;
    case EnemyState::Alive:
        // これまでの更新処理
        if(dir == Direction::Right){
            x += speed * delta;
        } else {
            x -= speed * delta;
        }

        if(x < leftBound) {
            x = leftBound;
            dir = Direction::Right;
        }
        if(x > rightBound) {
            x = rightBound;
            dir = Direction::Left;
        }
        // アニメーション処理
        anim.update(delta);
        sprite.setFrame(anim.getFrame());
        break;
    default:
        break;
    }
}

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Enemy::getCollisionRect() const{
    return {(int)x+40, (int)y+40, sprite.getDrawWidth()-40, sprite.getDrawHeight()-40};
}

/**
 * @brief Characterクラスの設定をオーバーライドして独自の実装を実施
 * 
 * 現状は点滅は描画スキップで実装している
 * 
 * @param renderer 
 * @param camera 
 */
void Enemy::draw(Renderer& renderer, Camera& camera){
    // 死亡時は何もしない
    if(state == EnemyState::Dead){
        return;
    }
    // Dying時は点滅
    if(state == EnemyState::Dying){
        constexpr double BLINK_INTERVAL = EnemyConfig::DYING_BLINK_INTERVAL;
        double t = std::fmod(dyingTime, BLINK_INTERVAL * 2.0);  // fmodで浮動小数点数の剰余算を実施
        bool visible = (t < BLINK_INTERVAL);
        // 描画しないフレームを作る
        if(!visible){
            return;
        }
    }
    // Alive || (Dying && visible)ならば通常描画
    Character::draw(renderer, camera);
}

/**
 * @brief リセット処理
 * 
 * 呼ばれたときに敵の座標/速度/向きを乱数生成器を用いて決定する
 * 
 * @param rd 
 * @param dx 
 * @param dy 
 * @param ds 
 */
void Enemy::reset(std::mt19937& rd,
                  std::uniform_real_distribution<double>& dx,
                  std::uniform_real_distribution<double>& dy,
                  std::uniform_real_distribution<double>& ds)
{
    setPosition(dx(rd), dy(rd));
    setSpeed(ds(rd));
    std::uniform_int_distribution<int> d01(0, 1);
    dir = (d01(rd) == 0 ? Direction::Left : Direction::Right);
    state = EnemyState::Alive;
    dyingTime = 0.0;
    anim.reset();
}

/**
 * @brief 状態管理用メソッド
 * 
 * Enemyの状態をDyingにして，各種のメンバ変数をDying用に設定する
 */
void Enemy::startDying(){
    // Dying/Deadのときは何もしない(多重で踏み潰さないように)
    if(state != EnemyState::Alive){
        return;
    }
    // Dyingの状態へ設定
    state = EnemyState::Dying;
    dyingTime = 0.0;
    vv = 0.0;
    speed = 0.0;
}

/**
 * @brief Enemyのx, y, speedをセットする
 * 
 * reset()やloadStage()などで呼ぶ想定
 * 
 * @param coorX 
 * @param coorY 
 * @param spd 
 */
void Enemy::applyEnemyParamForSpawn(double coorX, double coorY, double spd){
    setPosition(coorX, coorY);
    setSpeed(spd);
    // 固定でRight
    dir = Direction::Right;
    state = EnemyState::Alive;
    dyingTime = 0.0;
    vv = 0.0;
    anim.reset();
}
