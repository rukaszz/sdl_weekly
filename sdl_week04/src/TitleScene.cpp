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

}

/**
 * @brief タイトル画面の更新処理
 * 
 * @param delta 
 */
void TitleScene::update(double delta){
    // タイトルのフェードインなど
    game->updateTitle(delta);
    const Uint8* k = SDL_GetKeyboardState(NULL);
    if(k[SDL_SCANCODE_RETURN]){
        // state = GameState::Playing;
        score = 0;
        g->reset();
    }
    return;   // 画面のオブジェクトの更新はしない
}

/**
 * @brief タイトル画面の描画処理用
 * 
 * @param renderer 
 */
void TitleScene::render(Renderer& renderer){
    fpsText->draw(*renderer, 20, 20);
    // 中央にタイトル
    gameTitleText->draw(
        *renderer,
        GameConfig::WINDOW_WIDTH/2 - gameTitleText->getWidth()/2,
        GameConfig::WINDOW_HEIGHT/3 - gameTitleText->getHeight()/2
    );

    // 下に「Press ENTER to Start」
    if(blinkVisible){
        titleText->draw(
            *renderer,
            GameConfig::WINDOW_WIDTH/2 - titleText->getWidth()/2,
            GameConfig::WINDOW_HEIGHT/2 - titleText->getHeight()/2
        );
    }
}
