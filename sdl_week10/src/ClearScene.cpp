#include "GameConfig.hpp"
#include "ClearScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Clear Scene:: Clear Scene object
 * 
 * @param sc 
 * @param gc 
 */
ClearScene::ClearScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    )
{
    // クリア表示
    clearText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 216, 0, 255});
    clearText->setText("CLEAR!!");
    // 次のステージへ
    NextStageText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    NextStageText->setText("Press ENTER to Next Stage");
    // タイトルへ戻る
    returnTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    returnTitleText->setText("Press ESC to Title");
}

/**
 * @brief ゲームクリア時のイベント処理
 * 
 * @param e 
 */
void ClearScene::handleEvent(const SDL_Event& e){
    // キーイベントをInputクラスへ動かしたので今は何もしない
    (void)e;
}

/**
 * @brief ゲームクリア時の更新処理
 * ※Sceneクラス監視前にしていたゲームのリセット処理はhandleEventへ移管
 * 
 * @param delta 
 */
void ClearScene::update(double delta){
    // クリアの演出
    updateClear(delta);
    // Playingへの遷移
    const InputState& is = ctx.input.getState();
    // Enterでステージ遷移
    if(is.justPressed[(int)Action::Enter]){
        if(ctrl.goToNextStage()){
            // ステージインデックスを勧めてシーンを切り替える
            // ctrl.changeScene(GameScene::Playing);
            ctrl.requestScene(GameScene::Playing);
        } else {
            // 全ステージクリア
            // タイトルへ戻すだけ
            // 後でリザルト画面(ResultScene)を作る
            // ctrl.changeScene(GameScene::Title);
            ctrl.requestScene(GameScene::Title);
        }
    }
    // ESCでタイトル画面へ
    if(is.justPressed[(int)Action::Pause]){
        // ctrl.changeScene(GameScene::Title);
        ctrl.requestScene(GameScene::Title);
    }
}

/**
 * @brief ゲームクリア時の画面描画処理
 * 
 * @param remderer 
 */
void ClearScene::render(){
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    // CLEARは点滅
    if(blinkVisible){
        clearText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - clearText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/3 - clearText->getHeight()/2
        );
    }
    NextStageText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - NextStageText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/2 - NextStageText->getHeight()/2
    );
    returnTitleText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - returnTitleText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/1.5 - returnTitleText->getHeight()/2
    );
    
}

/**
 * @brief CLEARを点滅させる
 * 
 * @param delta 
 */
void ClearScene::updateClear(double delta){
    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}

/**
 * @brief ClearSceneへ入った際の初期化処理
 * 
 */
void ClearScene::onEnter(){
    blinkTimer = 0.0;
    blinkVisible = true;
}

/**
 * @brief ClearSceneから別シーンへ移る際の終了処理
 * 
 */
void ClearScene::onExit(){

}
