#include "SceneControl.hpp"

/**
 * @brief Construct a new Scene Control:: Scene Control object
 * 
 */
SceneControl::SceneControl(){

}

/**
 * @brief ゲームを始める関数
 * 
 * スコアリセットなど今のresetGameに相当
 * ここでscore, 残機, その他ゲーム全体の状態をリセット
 */
void SceneControl::startNewGame() {
    currentStageIndex = 0;
}

/**
 * @brief 次のステージへ進む関数
 * 
 * @return true：次のステージへ
 * @return false：全ステージクリア 
 */
bool SceneControl::goToNextStage() {
    ++currentStageIndex;
    if (currentStageIndex >= static_cast<int>(StageConfig::STAGES.size())) {
        // 全ステージクリア
        return false;
    }
    return true;
}

/**
 * @brief ステージを読み込む関数
 * 
 * PlayingSceceへ入るたびに呼ばれる
 * 
 * @param stageIndex 
 * @param ctx 
 */
void SceneControl::loadStage(int stageIndex, GameContext& ctx);
