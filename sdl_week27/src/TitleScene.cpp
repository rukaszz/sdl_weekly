#include "TitleScene.hpp"

#include <SDL2/SDL.h>

#include "Game.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "MusicId.hpp"
#include "SimpleSceneBackground.hpp"

// 定数
#include "GameConfig.hpp"

/**
 * @brief Construct a new Title Scene:: Title Scene object
 * 
 * @param sc 
 * @param gc 
 */
TitleScene::TitleScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    ), background(
        GameConfig::WINDOW_WIDTH, 
        GameConfig::WINDOW_HEIGHT
    )
{
    // タイトル
    gameTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.largeFont, SDL_Color{20, 20, 20, 255});
    gameTitleText->setText("The Mysterious Forest");
    // プロンプト
    // Enterでスタート
    gameEnterText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
    gameEnterText->setText("Press ENTER to Start");
    // Escで終了
    gameLeaveText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
    gameLeaveText->setText("Press ESC to Quit");
    // 操作方法
    howToPlayText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{20, 20, 20, 255});
    howToPlayText->setText("Move: Arrow Keys / Jump: Space / Fire: B / Pause: Esc");
}

/**
 * @brief タイトル画面でのイベント管理
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
    // Enterキー押下でPlayingへ遷移
    const InputState& is = ctx.input.getState();
    if(is.justPressed[static_cast<int>(Action::Enter)]){
        ctrl.startNewGame();
        ctx.events.requestScene(GameScene::Playing);
        return;
    }
    // Escキー押下でゲームを終了
    if(is.justPressed[static_cast<int>(Action::Pause)]){
        ctx.events.requestGameQuit();
        return;
    }
}

/**
 * @brief タイトル画面の描画処理用
 * 
 * @param renderer 
 */
void TitleScene::render(){
    // 背景描画
    background.render(ctx.renderer);
    // 背景のあとにテキスト
    if(GameConfig::SHOW_DEBUG_TEXT){
        ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    }
    // 中央にタイトル
    gameTitleText->draw(
        ctx.renderer,
        static_cast<int>(GameConfig::WINDOW_WIDTH*0.5 - gameTitleText->getWidth()/2),
        static_cast<int>(GameConfig::WINDOW_HEIGHT*0.3 - gameTitleText->getHeight()/2)
    );

    // 下に「Press ENTER to Start」
    if(blinkVisible){
        gameEnterText->draw(
            ctx.renderer,
            static_cast<int>(GameConfig::WINDOW_WIDTH*0.5 - gameEnterText->getWidth()/2),
            static_cast<int>(GameConfig::WINDOW_HEIGHT * 0.66)
        );
    }
    // Press Esc To Quit
    gameLeaveText->draw(
        ctx.renderer,
        static_cast<int>(GameConfig::WINDOW_WIDTH*0.5 - gameLeaveText->getWidth()/2),
        static_cast<int>(GameConfig::WINDOW_HEIGHT * 0.74)
    );
    // 画面下部に操作方法テキスト
    howToPlayText->draw(
        ctx.renderer,
        static_cast<int>(GameConfig::WINDOW_WIDTH*0.5 - howToPlayText->getWidth()/2),
        static_cast<int>(GameConfig::WINDOW_HEIGHT * 0.86)
    );
}

/**
 * @brief TitleSceneへ入った際の初期化処理
 * 
 */
void TitleScene::onEnter(){
    blinkTimer = 0.0;
    titleFade = 0.0;
    blinkVisible = true;
    // 背景読み込み
    background.setPreset(
        ctx.renderAssets.bgTextures, 
        BackgroundId::Forest
    );
    // BGM再生
    ctx.musicSystem.playIfChanged(MusicId::Title);
}

/**
 * @brief TitleSceneから別シーンへ移る際の終了処理
 * 
 */
void TitleScene::onExit(){
    background.clear();
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
