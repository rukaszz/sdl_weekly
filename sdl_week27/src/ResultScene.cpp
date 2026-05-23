#include "ResultScene.hpp"

#include <SDL2/SDL.h>

#include "Game.hpp"
#include "Renderer.hpp"
#include "MusicId.hpp"
#include "SimpleSceneBackground.hpp"

// 定数
#include "GameConfig.hpp"

/**
 * @brief Construct a new Result Scene:: Result Scene object
 * 
 * Result用UI初期化
 */
ResultScene::ResultScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    ), background(
        GameConfig::WINDOW_WIDTH, 
        GameConfig::WINDOW_HEIGHT
    )
{   
    // リザルト
    resultText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.largeFont, SDL_Color{20, 20, 20, 255});
    resultText->setText("Result");
    // Enterキーでタイトルへ
    returnTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
    returnTitleText->setText("Press Enter to Title");
    praiseText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255}); // 淡い黄色
    praiseText->setText("Congratulations!");
    // 動的テキスト
    stageText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
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
    if(is.justPressed[static_cast<int>(Action::Enter)]){
        // ctrl.requestScene(GameScene::Title);
        ctx.events.requestScene(GameScene::Title);
    }
}

/**
 * @brief リザルト画面の描画処理
 * 
 * @param remderer 
 */
void ResultScene::render(){
    // 背景描画
    background.render(ctx.renderer);
    // 背景のあとにテキスト
    if(GameConfig::SHOW_DEBUG_TEXT){
        ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    }
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
    ctx.textRenderCtx.scoreText.draw(
        ctx.renderer, 
        GameConfig::WINDOW_WIDTH/2 - ctx.textRenderCtx.scoreText.getWidth()/2, 
        GameConfig::WINDOW_HEIGHT/2);
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
 * @brief ResultSceneへ入った際の初期化処理
 * 
 */
void ResultScene::onEnter(){
    blinkTimer = 0.0;
    blinkVisible = true;
    const int total = ctrl.getStageCount();
    stageText->setText("Stage: " + std::to_string(total) + " / " + std::to_string(total));
    // 背景読み込み
    background.setPreset(
        ctx.renderAssets.bgTextures, 
        BackgroundId::lightBg
    );
    // BGMを再生する
    ctx.musicSystem.playIfChanged(MusicId::Result);
}

/**
 * @brief ResultSceneから別シーンへ移る際の終了処理
 * 
 */
void ResultScene::onExit(){
    background.clear();
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
