#include "CameraShakeController.hpp"

#include <algorithm>

#include "Camera.hpp"

// メンバ変数初期化関数
/**
 * @brief メンバ変数初期化関数
 * onEnter/onExitなどで呼ばれる
 * 
 */
void CameraShakeController::reset(){
    timer         = 0.0;
    duration      = 0.0;
    magnitude     = 0.0;
    shakeOffset_X = 0.0;
    shakeOffset_Y = 0.0;
}

/**
 * @brief 画面シェイク用の変数の設定関数
 * 
 * @param newDuration 
 * @param newMagnitude 
 */
void CameraShakeController::start(double newDuration, double newMagnitude){
    // 負の値は許容しない
    if(newDuration <= 0.0 || newMagnitude <= 0.0){
        return;
    }
    // 現在の画面シェイク強度を取得
    /* 現在設定されているシェイク間隔とシェイク時間が0より大きい
     * →単位あたりの揺れの強さを計算してセット
     * そうでなければ0をセット
     */
    const double currentStrength = (duration > 0.0 && timer > 0.0) 
        ? (magnitude * (timer/duration)) : 0.0;
    // 現在のシェイク強度より，呼び出しで要求されたマグニチュードが大きい場合
    if(newMagnitude >= currentStrength){
        // 各変数を更新する
        duration   = newDuration;
        timer      = newDuration;
        magnitude  = newMagnitude;
        return;
    }
    // 現在のシェイク強度より弱い場合は少しシェイク時間を延ばすだけ
    timer = std::max(timer, newDuration);
}

/**
 * @brief オフセット適用したカメラの値を返す
 * 
 * @param camera 
 * @return Camera 
 */
Camera CameraShakeController::applyToCamera(Camera camera) const{
    camera.x += shakeOffset_X;
    camera.y += shakeOffset_Y;
    return camera;
}
