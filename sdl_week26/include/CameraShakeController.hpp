#ifndef CAMERASHAKECONTROLLER_H
#define CAMERASHAKECONTROLLER_H

#include <algorithm>
#include <random>

#include "Camera.hpp"

/**
 * @brief 画面シェイク処理を管理するコントローラ
 * 
 */
class CameraShakeController{
private:
    double timer         = 0.0; // シェイク時間
    double duration      = 0.0; // シェイクの間隔
    double magnitude     = 0.0; // シェイクの強さ
    double shakeOffset_X = 0.0; // シェイクのX軸のズレ
    double shakeOffset_Y = 0.0; // シェイクのY軸のズレ
    // シェイク用ランダム分布
    std::uniform_real_distribution<double> dist{-1.0, 1.0};

public:
    CameraShakeController() = default;
    ~CameraShakeController() = default;
    // メンバ変数初期化関数
    void reset();
    // 画面シェイク用の変数の設定関数
    void start(double newDuration, double newMagnitude);
    // カメラの値へのオフセット適用
    Camera applyToCamera(Camera camera) const;
    /**
     * @brief 画面シェイク用のオフセット設定関数
     * 
     * @tparam RandomEngine 
     * @param delta 
     * @param rng 
     */
    template<typename RandomEngine>
    void update(double delta, RandomEngine& rng){
        // タイマーが0以下なら一旦リセット
        if(timer <= 0.0){
            reset();
            return;
        }
        // タイマーの減衰
        timer = std::max(0.0, timer - delta);
        // シェイクの減衰割合
        const double ratio = (duration > 0.0) ? (timer / duration) : 0.0;
        // 現在のシェイクの強さ
        const double currentMag = magnitude * ratio;

        // X, Y軸の揺れをセット
        shakeOffset_X = dist(rng) * currentMag;
        shakeOffset_Y = dist(rng) * currentMag * 0.5; // 縦揺れは若干控えめ
    }
};

#endif  // CAMERASHAKECONTROLLER_H
