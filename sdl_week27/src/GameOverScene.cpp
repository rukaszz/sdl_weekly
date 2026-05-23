#include "GameOverScene.hpp"

#include <SDL2/SDL.h>

#include "Game.hpp"
#include "Renderer.hpp"
#include "MusicId.hpp"
#include "SimpleSceneBackground.hpp"

// 定数
#include "GameConfig.hpp"

/**
 * @brief Construct a new Game Over Scene:: Game Over Scene object
 * 
 */
GameOverScene::GameOverScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    ), background(
        GameConfig::WINDOW_WIDTH, 
        GameConfig::WINDOW_HEIGHT
    )
{
    // 定型文
    gameOverText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.largeFont, SDL_Color{255, 40, 40, 255});
    gameOverText->setText("GameOver!!");
    youAreDeadText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.largeFont, SDL_Color{255, 128, 128, 255});
    youAreDeadText->setText("You are Dead!");
    returnTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    returnTitleText->setText("Press ESC to Title");
    returnStageText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    returnStageText->setText("Press ENTER to Return Stage");
    // 動的文字列
    remainingLivesText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
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
 * 
 * @param delta 
 */
void GameOverScene::update(double delta){
    // GameOverの演出
    updateGameOver(delta);
    // Playingへの遷移
    const InputState& is = ctx.input.getState();
    // Enterでリトライ
    if(is.justPressed[static_cast<int>(Action::Enter)]){
        // ctrl.requestScene(GameScene::Playing);
        if(ctrl.tryConsumeLife()){
            ctx.events.requestScene(GameScene::Playing);    
        } else {
            ctx.events.requestScene(GameScene::Title);
        }
    }
    // Pauseでタイトル画面へ
    // 諦めてタイトルへ戻るので，リザルト画面(ResultScene)へ遷移する予定
    if(is.justPressed[static_cast<int>(Action::Pause)]){
        // ctrl.requestScene(GameScene::Title);
        ctx.events.requestScene(GameScene::Title);
    }
}

/**
 * @brief ゲームオーバー時の画面描画処理
 * 
 */
void GameOverScene::render(){
    // 背景
    background.render(ctx.renderer);
    // 背景描画後にテキスト
    if(GameConfig::SHOW_DEBUG_TEXT){
        ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    }
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    // 残機がなければGameOver
    if(remainingLives <= 0){
        // 点滅
        if(blinkVisible){
            gameOverText->draw(
                ctx.renderer,
                GameConfig::WINDOW_WIDTH/2 - gameOverText->getWidth()/2,
                GameConfig::WINDOW_HEIGHT/3 - gameOverText->getHeight()/2
            );
        }
    } else {
        // 点滅
        if(blinkVisible){
            youAreDeadText->draw(
                ctx.renderer,
                GameConfig::WINDOW_WIDTH/2 - youAreDeadText->getWidth()/2,
                GameConfig::WINDOW_HEIGHT/3 - youAreDeadText->getHeight()/2
            );
        }
    }
    returnTitleText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - returnTitleText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/2 - returnTitleText->getHeight()/2
    );
    // コンティニュー可能な残機数ならreturnStageTextを表示
    if(remainingLives > 0){
        returnStageText->draw(
            ctx.renderer,
            GameConfig::WINDOW_WIDTH/2 - returnStageText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/1.5 - returnStageText->getHeight()/2
        );
    }
    remainingLivesText->draw(
        ctx.renderer,
        GameConfig::WINDOW_WIDTH/2 - remainingLivesText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/1.25 - remainingLivesText->getHeight()/2
    );
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
    // 変数初期化
    blinkTimer = 0.0;
    blinkVisible = true;
    // 背景読み込み
    background.setPreset(
        ctx.renderAssets.bgTextures, 
        BackgroundId::darkBg
    );
    // このシーンに入った時点の残機で文字列を作る
    remainingLives = ctrl.getLives();
    remainingLivesText->setText("Lives(Retry Left): " + std::to_string(remainingLives));
    // GameOver時のスコアペナルティ
    ctx.events.addScore(GameConfig::SCORE_PENALTY);
    // BGM再生
    ctx.musicSystem.playIfChanged(MusicId::GameOver);
}

/**
 * @brief GameOverSceneから別シーンへ移る際の終了処理
 * 
 */
void GameOverScene::onExit(){
    background.clear();
    // Player死亡→リトライ時にPlayerFormなどをリセットする
    ctx.entityCtx.player.resetForNewGame();
}
