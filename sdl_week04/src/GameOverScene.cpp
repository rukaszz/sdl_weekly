#include "GameOverScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Game Over Scene:: Game Over Scene object
 * 
 */
GameOverScene::GameOverScene()
    : Scene(g)
{

}

/**
 * @brief ゲームオーバー時の更新処理
 * 
 * @param delta 
 */
void GameOverScene::update(double delta){
    const Uint8* k = SDL_GetKeyboardState(NULL);
    if(k[SDL_SCANCODE_RETURN]){
        reset();    // PlayerとEnemyを元の位置へ戻す
        state = GameState::Playing;
    }
}

/**
 * @brief ゲームオーバー時の画面描画処理
 * 
 * @param remderer 
 */
void GameOverScene::render(Renderer& remderer){
    fpsText->draw(*renderer, 20, 20);
    scoreText->draw(*renderer, 20, 50);
    gameOverText->draw(
        *renderer,
        GameConfig::WINDOW_WIDTH/2 - gameOverText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/2 - gameOverText->getHeight()/2
    );
}
