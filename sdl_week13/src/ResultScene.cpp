#include "GameConfig.hpp"
#include "StageConfig.hpp"
#include "ResultScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Result Scene:: Result Scene object
 * 
 * Result用UI初期化
 */
ResultScene::ResultScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    )
{
    resultText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    resultText->setText("Result");
    returnTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    returnTitleText->setText("Press Enter to Title");
    praiseText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{244, 229, 17, 255}); // 淡い黄色
    praiseText->setText("Congratulations!");
    stageText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
}

/**
 * @brief 全ステージクリア時のリザルト画面のイベント処理
 * 
 * @param e 
 */
void ResultScene::handleEvent(const SDL_Event& e){
    // キーイベントをInputクラスへ動かしたので今は何もしない
    (void)e;
}

/**
 * @brief リザルト画面の更新処理
 * 
 * @param delta 
 */
void ResultScene::update(double delta){
    // 点滅の演出
    updateBlink(delta);
    // input取得
    const InputState& is = ctx.input.getState();
    // Enterでタイトル画面へ(Titleへ進むのでEnter)
    if(is.justPressed[(int)Action::Enter]){
        ctrl.requestScene(GameScene::Title);
    }
}

/**
 * @brief ゲームオーバー時の画面描画処理
 * 
 * @param remderer 
 */
void ResultScene::render(){
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    // ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    // Result
    resultText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - resultText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/4 - resultText->getHeight()/2
    );
    praiseText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - praiseText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/3 - praiseText->getHeight()/2
    );
    ctx.textRenderCtx.scoreText.draw(ctx.renderer
                                   , GameConfig::WINDOW_WIDTH/2 - ctx.textRenderCtx.scoreText.getWidth()/2
                                   , GameConfig::WINDOW_HEIGHT/2);
    stageText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - stageText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/1.75 - stageText->getHeight()/2
    );
    // Titleシーンへの遷移を点滅させる
    if(blinkVisible){
        returnTitleText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - returnTitleText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/1.25 - returnTitleText->getHeight()/2
        );
    }
}

/**
 * @brief テキストを点滅させる
 * 
 * @param delta 
 */
void ResultScene::updateBlink(double delta){
    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}

/**
 * @brief ResultSceneへ入った際の初期化処理
 * 
 */
void ResultScene::onEnter(){
    blinkTimer = 0.0;
    blinkVisible = true;
    const auto cleared = ctrl.getCurrentStageIndex(); // == total
    const auto total   = StageConfig::STAGES.size();
    stageText->setText("Stage: " + std::to_string(cleared) + " / " + std::to_string(total));
}

/**
 * @brief ResultSceneから別シーンへ移る際の終了処理
 * 
 */
void ResultScene::onExit(){

}
