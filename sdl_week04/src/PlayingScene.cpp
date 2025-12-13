#include "PlayingScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Playing Scene:: Playing Scene object
 * 
 */
PlayingScene::PlayingScene()
    : Scene(g)
{

}

/**
 * @brief ゲーム画面の更新処理
 * 
 * @param delta 
 */
void PlayingScene::update(double delta){
    // ウィンドウサイズの型変換
    SDL_Point p = window->getWindowSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    // スコア計算
    score += delta * GameConfig::SCORE_RATE;  // 生存時間に重点
    scoreText->setText("Score: " + std::to_string(static_cast<int>(score)));
    // 更新
    player->update(delta, b);
    for(auto& e : enemies) e->update(delta, b);
    // 衝突判定
    for(auto& e : enemies){
        if(GameUtil::intersects(player->getCollisionRect(), e->getCollisionRect())){
            state = GameState::GameOver;
        }
    }
}

/**
 * @brief ゲーム中の画面描画処理
 * 
 * @param renderer 
 */
void PlayingScene::render(Renderer& renderer){
    fpsText->draw(*renderer, 20, 20);
    scoreText->draw(*renderer, 20, 50);
    player->draw(*renderer);
    for(auto& e : enemies) e->draw(*renderer);
}
