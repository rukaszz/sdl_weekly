#include "GameOverScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Game Over Scene:: Game Over Scene object
 * 
 */
GameOverScene::GameOverScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    )
{
    gameOverText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    gameOverText->setText("GameOver");
}

/**
 * @brief ゲームオーバー時のイベント処理
 * Enterキー押下でリセットする(TitleではなくPlayingへ戻す)
 * 
 * @param e 
 */
void GameOverScene::handleEvent(const SDL_Event& e){
    // キーイベントをInputクラスへ動かしたので今は何もしない
    (void)e;
}

/**
 * @brief ゲームオーバー時の更新処理
 * ※Sceneクラス監視前にしていたゲームのリセット処理はhandleEventへ移管
 * 
 * @param delta 
 */
void GameOverScene::update(double delta){
    // GameOverの演出
    updateGameOver(delta);
    // Playingへの遷移
    const InputState& is = ctx.input.getState();
    if(is.justPressed[(int)Action::Enter]){
        ctrl.resetGame();
        ctrl.changeScene(GameScene::Playing);
        return;
    }
}

/**
 * @brief ゲームオーバー時の画面描画処理
 * 
 * @param remderer 
 */
void GameOverScene::render(){
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    if(blinkVisible){
        gameOverText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - gameOverText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/2 - gameOverText->getHeight()/2
        );
    }
}

/**
 * @brief GAMEOVERを点滅させる
 * 
 * @param delta 
 */
void GameOverScene::updateGameOver(double delta){
    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}

/**
 * @brief GameOverSceneへ入った際の初期化処理
 * 
 */
void GameOverScene::onEnter(){
    blinkTimer = 0.0;
    blinkVisible = true;
}

/**
 * @brief GameOverSceneから別シーンへ移る際の終了処理
 * 
 */
void GameOverScene::onExit(){

}
