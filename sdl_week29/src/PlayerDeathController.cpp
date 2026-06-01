#include "PlayerDeathController.hpp"

/**
 * @brief プレイヤー死亡演出イベントの開始
 * 
 */
void PlayerDeathController::start(){
    // 各種変数の設定
    deathTimer   = DURATION;
    blinkTimer   = 0.0;
    blinkVisible = true;
    active       = true; // 当該イベントの有効化
}

/**
 * @brief プレイヤー死亡演出の更新処理
 * 
 * @param delta 
 * @return true：GameOverSceneへ遷移
 * @return false：無効化中 or 演出中
 */
bool PlayerDeathController::update(double delta){
    // 当該イベントが無効化中は何もしない
    if(!active){
        return false;
    }
    // 点滅タイマーの更新
    blinkTimer += delta;
    // 点滅の間隔(インターバル)を超えたら
    if(blinkTimer >= BLINK_INTERVAL){
        blinkTimer   = 0.0;             // 計測を初期化
        blinkVisible = !blinkVisible;   // 点滅の状態を反転
    }
    // 演出タイマーの更新
    deathTimer -= delta;
    // 死亡演出時間が0以下になったら
    if(deathTimer <= 0){
        active = false; // 当該イベントの無効化
        return true;    // trueを返して終了を通知
    }
    // 演出中はfalseを返す
    return false;
}

/**
 * @brief 各種変数を初期化
 * 
 */
void PlayerDeathController::reset(){
    deathTimer   = 0.0;
    blinkTimer   = 0.0;
    blinkVisible = true;
    active       = false;    // 当該イベントは無効化
}
