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
    game.updateTitle(delta);
    const Uint8* k = SDL_GetKeyboardState(NULL);
    if(k[SDL_SCANCODE_RETURN]){
        // scene = GameScene::Playing;
        game.setScore(0);
        game.reset();
    }
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
