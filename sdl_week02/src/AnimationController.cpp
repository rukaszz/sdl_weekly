#include "AnimationController.hpp"

/**
 * @brief Construct a new Animation Controller:: Animation Controller object
 * 
 * @param maxFrames 
 * @param interval 
 */
AnimationController::AnimationController(int maxFrames, double interval)
    : maxFrames(maxFrames), interval(interval)
{

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