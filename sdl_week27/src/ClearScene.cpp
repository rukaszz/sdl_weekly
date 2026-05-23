#include "ClearScene.hpp"

#include <SDL2/SDL.h>

#include "Game.hpp"
#include "Renderer.hpp"
#include "MusicId.hpp"
#include "SimpleSceneBackground.hpp"

// 定数
#include "GameConfig.hpp"

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
    ), background(
        GameConfig::WINDOW_WIDTH, 
        GameConfig::WINDOW_HEIGHT
    )
{
    // クリア表示
    clearText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.largeFont, SDL_Color{255, 200, 20, 255});
    clearText->setText("CLEAR!!");
    // 次のステージへ
    nextStageText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
    nextStageText->setText("Press ENTER to Next Stage");
    // タイトルへ戻る
    returnTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
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
    if(is.justPressed[static_cast<int>(Action::Enter)]){
        if(ctrl.goToNextStage()){
            // ステージインデックスを勧めてシーンを切り替える
            ctx.events.requestScene(GameScene::Playing);
        } else {
            // 全ステージクリア
            // タイトルへ戻すだけ
            // 後でリザルト画面(ResultScene)を作る
            ctx.events.requestScene(GameScene::Result);
        }
    }
    // ESCでタイトル画面へ
    if(is.justPressed[static_cast<int>(Action::Pause)]){
        // タイトルへ戻るためにEscが押されたのでTitleScene
        ctx.events.requestScene(GameScene::Title);
    }
}

/**
 * @brief ゲームクリア時の画面描画処理
 * 
 * @param remderer 
 */
void ClearScene::render(){
    // 背景描画
    background.render(ctx.renderer);
    // 背景描画後にテキスト
    if(GameConfig::SHOW_DEBUG_TEXT){
        ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    }
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    // CLEARは点滅
    if(blinkVisible){
        clearText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - clearText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/3 - clearText->getHeight()/2
        );
    }
    nextStageText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - nextStageText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/2 - nextStageText->getHeight()/2
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
    // 背景読み込み
    background.setPreset(
        ctx.renderAssets.bgTextures, 
        BackgroundId::LightBg
    );
    // BGM再生
    ctx.musicSystem.playIfChanged(MusicId::Clear);
}

/**
 * @brief ClearSceneから別シーンへ移る際の終了処理
 * 
 */
void ClearScene::onExit(){
    background.clear();
}
