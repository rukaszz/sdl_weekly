#include "GameOverScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Game Over Scene:: Game Over Scene object
 * 
 */
GameOverScene::GameOverScene(Game& g)
    : Scene(g)
{

}

/**
 * @brief ゲームオーバー時のイベント処理
 * Enterキー押下でリセットする(TitleではなくPlayingへ戻す)
 * 
 * @param e 
 */
void GameOverScene::handleEvent(const SDL_Event& e){
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN){
        game.reset();
        game.setGameScene(GameScene::Playing);
    }
}

/**
 * @brief ゲームオーバー時の更新処理
 * ※Sceneクラス監視前にしていたゲームのリセット処理はhandleEventへ移管
 * 
 * @param delta 
 */
void GameOverScene::update(double delta){
    
}

/**
 * @brief ゲームオーバー時の画面描画処理
 * 
 * @param remderer 
 */
void GameOverScene::render(){
    game.getFpsText().draw(game.getRenderer(), 20, 20);
    game.getScoreText().draw(game.getRenderer(), 20, 50);
    game.getGameOverText().draw(
        game.getRenderer(),
        GameConfig::WINDOW_WIDTH/2 - game.getGameOverText().getWidth()/2,
        GameConfig::WINDOW_HEIGHT/2 - game.getGameOverText().getHeight()/2
    );
}
