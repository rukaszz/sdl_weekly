#include "GameUtil.hpp"
#include "PlayingScene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Playing Scene:: Playing Scene object
 * 
 */
PlayingScene::PlayingScene(SceneControl& sc, GameContext& gc)
    : Scene(
        sc, 
        gc
    )
{

}

/**
 * @brief プレイ中の画面のイベント
 * プレイヤー操作などはPlayerのupdate内で管理する
 * ※連続するような入力はイベントに依存させない
 * 
 * @param e 
 */
void PlayingScene::handleEvent(const SDL_Event& e){
    // キーイベントをInputクラスへ動かしたので今は何もしない
    (void)e;
}

/**
 * @brief ゲーム画面の更新処理
 * 
 * @param delta 
 */
void PlayingScene::update(double delta){
    // Titleへの遷移
    const InputState& is = ctx.input.getState();
    if(is.justPressed[(int)Action::Pause]){
        ctrl.changeScene(GameScene::Title);
        return;
    }
    // ウィンドウサイズの型変換
    SDL_Point p = ctx.renderer.getOutputSize();
    DrawBounds b = {static_cast<double>(p.x), static_cast<double>(p.y)};
    
    updateScore(delta);
    updateEntities(delta, b);
    checkCollision();
    hasFallenToGameOver();
}

/**
 * @brief ゲーム中の画面描画処理
 * 
 */
void PlayingScene::render(){
    // テキスト描画
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);
    // ブロック描画
    for(const auto& b : ctx.entityCtx.blocks){
        SDL_Color blockColor;
        // 床のタイプで描画を変更
        switch(b.type){
        case BlockType::Standable:
            blockColor = {255, 255, 255, 255};  // 白
            break;
        case BlockType::DropThrough:
            blockColor = {128, 128, 255, 255};  // 青
            break;
        case BlockType::Damage:
            blockColor = {255, 0, 0, 255};      // 赤
            break;
        case BlockType::Clear:
            blockColor = {255, 216, 0, 255};    // 黃
            break;
        }
        SDL_Rect r = {static_cast<int>(b.x), static_cast<int>(b.y),
                      static_cast<int>(b.w), static_cast<int>(b.h)};
        ctx.renderer.drawRect(r, blockColor);
    }
    // キャラクタ描画
    ctx.entityCtx.player.draw(ctx.renderer);
    for(auto& e : ctx.entityCtx.enemies) e->draw(ctx.renderer);
}

/**
 * @brief PlayingSceneへ入った際の初期化処理
 * 
 */
void PlayingScene::onEnter(){
    ctrl.resetGame();
    // ctx.blocks.clear();
    // 床ブロック配置
    // ctx.blocks.push_back(Block);
}

/**
 * @brief PlayingSceneから別シーンへ移る際の終了処理
 * 
 */
void PlayingScene::onExit(){

}

/**
 * @brief スコア算出処理
 * 
 */
void PlayingScene::updateScore(double delta){
    // スコア計算
    uint32_t s = ctrl.getScore() + delta * GameConfig::SCORE_RATE; // 生存時間に重点
    ctrl.setScore(s);  
    ctx.textRenderCtx.scoreText.setText("Score: " + std::to_string(static_cast<int>(ctrl.getScore())));
}

/**
 * @brief 描画するオブジェクトの更新処理
 * Player/Enemyなど
 * 
 */
void PlayingScene::updateEntities(double delta, DrawBounds b){
    // キーの状態取得
    const InputState& is = ctx.input.getState();
    // キャラクタの更新
    // ctx.player.update(delta, is, b);
    ctx.entityCtx.player.update(delta, is, b, ctx.entityCtx.blocks);
    for(auto& e : ctx.entityCtx.enemies) e->update(delta, is, b, ctx.entityCtx.blocks);
}

/**
 * @brief 描画するオブジェクトの衝突処理
 * 
 */
void PlayingScene::checkCollision(){
    // 敵との衝突判定
    for(auto& e : ctx.entityCtx.enemies){
        if(GameUtil::intersects(ctx.entityCtx.player.getCollisionRect(), e->getCollisionRect())){
            ctrl.changeScene(GameScene::GameOver);
            return;
        }
    }
    // ダメージブロックとの衝突判定
    SDL_Rect playerRect = ctx.entityCtx.player.getCollisionRect();
    for(const auto& b : ctx.entityCtx.blocks){
        // ダメージ床, クリアオブジェクト以外は判定しない
        if(b.type == BlockType::Damage || b.type == BlockType::Clear){
            SDL_Rect br = GameUtil::blockToRect(b);
            if(GameUtil::intersects(playerRect, br)){
                if(b.type == BlockType::Damage){
                    ctrl.changeScene(GameScene::GameOver);
                    return;
                }
                if(b.type == BlockType::Clear){
                    ctrl.changeScene(GameScene::Clear);
                    return;
                }
            }
        } else {
            continue;
        }
    }
}

/**
 * @brief 落下死判定関数
 * 
 */
void PlayingScene::hasFallenToGameOver(){
    double death_Y = GameConfig::WINDOW_HEIGHT + PlayerConfig::FRAME_H; // 画面外へ落下死たら終了
    double playerBottom = ctx.entityCtx.player.getCollisionRect().y
                          + ctx.entityCtx.player.getCollisionRect().h;
    if (playerBottom > death_Y){
        ctrl.changeScene(GameScene::GameOver);
        return;
    }
}
