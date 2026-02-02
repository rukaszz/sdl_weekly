// 定数
#include "GameConfig.hpp"

#include "TitleScene.hpp"
#include "Game.hpp"
#include "SceneControl.hpp"
#include "Renderer.hpp"
#include "Input.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Title Scene:: Title Scene object
 * 
 * @param g 
 */
TitleScene::TitleScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    )
{
    // タイトル
    gameTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    gameTitleText->setText("My Game");
    // プロンプト
    gameEnterText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    gameEnterText->setText("Press ENTER to Start");
}

/**
 * @brief タイトル画面でのイベント管理
 * 管理するイベントはエンターキーの押下のみ
 * 
 * @param e 
 */
void TitleScene::handleEvent(const SDL_Event& e){
    // キーイベントをInputクラスへ動かしたので今は何もしない
    (void)e;
}

/**
 * @brief タイトル画面の更新処理
 * 
 * @param delta 
 */
void TitleScene::update(double delta){
    // タイトルのフェードインなど
    updateTitle(delta);
    // Playingへ遷移
    const InputState& is = ctx.input.getState();
    if(is.justPressed[(int)Action::Enter]){
        ctrl.startNewGame();
        ctrl.resetGame();
        ctrl.requestScene(GameScene::Playing);
        return;
    }
}

/**
 * @brief タイトル画面の描画処理用
 * 
 * @param renderer 
 */
void TitleScene::render(){
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    // 中央にタイトル
    gameTitleText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - gameTitleText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/3 - gameTitleText->getHeight()/2
    );

    // 下に「Press ENTER to Start」
    if(blinkVisible){
        gameEnterText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - gameEnterText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/2 - gameEnterText->getHeight()/2
        );
    }
}

/**
 * @brief TitleSceneへ入った際の初期化処理
 * 
 */
void TitleScene::onEnter(){
    blinkTimer = 0.0;
    titleFade = 0;
    blinkVisible = true;
    // ctrl.resetGame(); // リセットはstartNewGame()呼び出し時のみ
}

/**
 * @brief TitleSceneから別シーンへ移る際の終了処理
 * 
 */
void TitleScene::onExit(){

}

/**
 * @brief タイトルをフェードイン＆点滅させる
 * 
 * @param delta 
 */
void TitleScene::updateTitle(double delta){
    /* ----- フェードイン ----- */
    if(titleFade < 1.0){
        // 2秒
        titleFade += delta * 0.5;
        if(titleFade > 1.0){
            titleFade = 1.0;    // 1.0を超えないように
        }
        // 徐々に実体化
        Uint8 alpha = static_cast<Uint8>(titleFade * 255);
        gameTitleText->setAlpha(alpha);
    }

    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}
