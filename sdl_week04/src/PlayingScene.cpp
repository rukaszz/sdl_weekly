#include "GameUtil.hpp"
#include "PlayingScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Playing Scene:: Playing Scene object
 * 
 */
PlayingScene::PlayingScene(Game& g)
    : Scene(g)
{

}

/**
 * @brief プレイ中の画面のイベント
 * プレイヤー操作などはPlayerのupdate内で管理する
 * ※連続するような入力はイベントに依存させない
 * 
 * @param e 
 */
void PlayingScene::handleEvent(const SDL_Event& e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_ESCAPE){
            game.changeScene(GameScene::Title);
        }
    }
}

/**
 * @brief ゲーム画面の更新処理
 * 
 * @param delta 
 */
void PlayingScene::update(double delta){
    // ウィンドウサイズの型変換
    SDL_Point p = game.getWindow().getWindowSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    // スコア計算
    uint32_t s = game.getScore() + delta * GameConfig::SCORE_RATE; // 生存時間に重点
    game.setScore(s);  
    game.getScoreText().setText("Score: " + std::to_string(static_cast<int>(game.getScore())));
    // 更新
    game.getPlayer().update(delta, b);
    for(auto& e : game.getEnemies()) e->update(delta, b);
    // 衝突判定
    for(auto& e : game.getEnemies()){
        if(GameUtil::intersects(game.getPlayer().getCollisionRect(), e->getCollisionRect())){
            game.setGameScene(GameScene::GameOver);
        }
    }
}

/**
 * @brief ゲーム中の画面描画処理
 * 
 * @param renderer 
 */
void PlayingScene::render(){
    game.getFpsText().draw(game.getRenderer(), 20, 20);
    game.getScoreText().draw(game.getRenderer(), 20, 50);
    game.getPlayer().draw(game.getRenderer());
    for(auto& e : game.getEnemies()) e->draw(game.getRenderer());
}
