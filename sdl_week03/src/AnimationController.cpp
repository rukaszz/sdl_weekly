#include "AnimationController.hpp"

/**
 * @brief オブジェクトからアニメーション処理を引き剥がす
 * 
 */

/**
 * @brief Construct a new Animation Controller:: Animation Controller object
 * 
 * @param maxFrames 
 * @param interval 
 */
AnimationController::AnimationController(int maxFrames, double interval)
    : maxFrames(maxFrames), interval(interval)
{
    // スプライトシート差し替え時の事故防止
    assert(maxFrames <= 0);
    assert(interval <= 0);
}

/**
 * @brief フレームを一定にしつつ更新する
 * 
 * @param delta 
 */
void AnimationController::update(double delta){
    timer += delta;
    if(timer >= interval){
        frame = (frame + 1) % maxFrames;
        timer -= interval;
    }
}