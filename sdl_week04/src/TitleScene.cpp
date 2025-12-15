// 定数
#include "GameConfig.hpp"

#include "TitleScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Title Scene:: Title Scene object
 * 
 * @param g 
 */
TitleScene::TitleScene(Game& g)
    : Scene(g)
{

}

/**
 * @brief タイトル画面でのイベント管理
 * 管理するイベントはエンターキーの押下のみ
 * 
 * @param e 
 */
void TitleScene::handleEvent(const SDL_Event& e){
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN){
        game.changeScene(GameScene::Playing);
    }
}

/**
 * @brief タイトル画面の更新処理
 * 
 * @param delta 
 */
void TitleScene::update(double delta){
    // タイトルのフェードインなど
    updateTitle(delta);
    // scene = GameScene::Playing;
    // game.setScore(0);
    // game.reset();
    
    return;   // 画面のオブジェクトの更新はしない
}

/**
 * @brief タイトル画面の描画処理用
 * 
 * @param renderer 
 */
void TitleScene::render(){
    game.getFpsText().draw(game.getRenderer(), 20, 20);
    // 中央にタイトル
    game.getGameTitleText().draw(
        game.getRenderer(),
        GameConfig::WINDOW_WIDTH/2 - game.getGameTitleText().getWidth()/2,
        GameConfig::WINDOW_HEIGHT/3 - game.getGameTitleText().getHeight()/2
    );

    // 下に「Press ENTER to Start」
    if(blinkVisible){
        game.getTitleText().draw(
            game.getRenderer(),
            GameConfig::WINDOW_WIDTH/2 - game.getTitleText().getWidth()/2,
            GameConfig::WINDOW_HEIGHT/2 - game.getTitleText().getHeight()/2
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
        game.getGameTitleText().setAlpha(alpha);
    }

    /* ----- 点滅 ----- */
    blinkTimer += delta;
    // 0.5秒ごとに切り替える
    if(blinkTimer >= 0.5){
        blinkTimer = 0;
        blinkVisible = !blinkVisible;
    }
}
