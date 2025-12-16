#include "GameUtil.hpp"
#include "PlayingScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Playing Scene:: Playing Scene object
 * 
 */
PlayingScene::PlayingScene(Game& g, const GameContext& context)
    : Scene(
        g, 
        context
    )
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
        switch(e.key.keysym.sym){
        case SDLK_ESCAPE:
            game.changeScene(GameScene::Title);
            break;
        default:
            break;
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
    // SDL_Point p = ctx.window.getWindowSize();
    SDL_Point p = ctx.renderer.getOutputSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    
    updateScore(delta);
    updateEntities(delta, b);
    checkCollision();
}

/**
 * @brief ゲーム中の画面描画処理
 * 
 * @param renderer 
 */
void PlayingScene::render(){
    ctx.fpsText.draw(ctx.renderer, 20, 20);
    ctx.scoreText.draw(ctx.renderer, 20, 50);
    ctx.player.draw(ctx.renderer);
    for(auto& e : ctx.enemies) e->draw(ctx.renderer);
}

/**
 * @brief PlayingSceneへ入った際の初期化処理
 * 
 */
void PlayingScene::onEnter(){
    game.reset();
}

/**
 * @brief PlayingSceneから別シーンへ移る際の終了処理
 * 
 */
void PlayingScene::onExit(){

}

/**
 * @brief スコア算出処理
 * 
 */
void PlayingScene::updateScore(double delta){
    // スコア計算
    uint32_t s = game.getScore() + delta * GameConfig::SCORE_RATE; // 生存時間に重点
    game.setScore(s);  
    ctx.scoreText.setText("Score: " + std::to_string(static_cast<int>(game.getScore())));
}

/**
 * @brief 描画するオブジェクトの更新処理
 * Player/Enemyなど
 * 
 */
void PlayingScene::updateEntities(double delta, DrawBounds b){
    // キャラクタの更新
    ctx.player.update(delta, b);
    for(auto& e : ctx.enemies) e->update(delta, b);
}

/**
 * @brief 描画するオブジェクトの衝突処理
 * 
 */
void PlayingScene::checkCollision(){
    // 衝突判定
    for(auto& e : ctx.enemies){
        if(GameUtil::intersects(ctx.player.getCollisionRect(), e->getCollisionRect())){
            game.changeScene(GameScene::GameOver);
            return;
        }
    }
}
