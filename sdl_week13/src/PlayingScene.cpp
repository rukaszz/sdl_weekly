#include "PlayingScene.hpp"
#include "PlayerEnemyCollisionUtil.hpp"
#include "Renderer.hpp"
#include "Game.hpp"
#include "GameUtil.hpp"

#include "GameConfig.hpp"
#include "EnemyConfig.hpp"
#include "FireBallConfig.hpp"

#include <SDL2/SDL.h>

#include <algorithm>

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
    debugText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 0, 255, 255});
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
    // エスケープキーでTitleへの遷移
    const InputState& is = ctx.input.getState();
    if(is.justPressed[(int)Action::Pause]){
        ctrl.requestScene(GameScene::Title);
        return;
    }
    if(is.justPressed[static_cast<int>(Action::Fire)]){
        double spawn_X = ctx.entityCtx.player.getEntityCenter_X();
        double spawn_Y = ctx.entityCtx.player.getEntityCenter_Y();
        Direction dir = ctx.entityCtx.player.getDirection();

        ctx.entityCtx.fireballs.emplace_back(
            std::make_unique<FireBall>(spawn_X, spawn_Y, dir, ctx.entityCtx.fireballTexture)
        );
    }
    // 1. 衝突処理用の前フレームのプレイヤー下部をサンプリング
    // 必ず各種Collision判定前に呼ぶ必要がある
    // 呼び出し順序に注意すること
    ctx.entityCtx.player.beginFrameCollisionSample();
    // 2. worldInfoを用いた幅のクランプ処理
    DrawBounds worldBounds = {ctx.worldInfo.WorldWidth, ctx.worldInfo.WorldHeight};
    // 3. スコア更新
    updateScore(delta);
    // 4. Characterの更新
    updateEntities(delta, worldBounds);
    // 5. Playerとの当たり判定
    detectCollision();
    // 6. 落下死判定
    hasFallenToGameOver();
    // 7. カメラ座標の更新
    updateCamera();
    // 8. ファイアボールの片付け
    cleanupFireBalls();
    // デバッグ情報取得
    debugText->setText(ctx.entityCtx.player.debugMoveContext());
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
        ctx.renderer.drawRect(r, blockColor, ctx.camera);
    }
    for(const auto& f : ctx.entityCtx.fireballs){
        f->draw(ctx.renderer, ctx.camera);
    }
    // キャラクタ描画
    // カメラを考慮した書き方にする
    ctx.entityCtx.player.draw(ctx.renderer, ctx.camera);
    for(auto& e : ctx.entityCtx.enemies) e->draw(ctx.renderer, ctx.camera);
    // デバッグ情報表示
    debugText->draw(ctx.renderer, 20, 80);
}

/**
 * @brief camera更新用関数
 * Playerがカメラの中心に来る
 * ただしclamp処理があるため，画面端ではplayerに追従しない
 * 
 */
void PlayingScene::updateCamera(){
    auto& player = ctx.entityCtx.player;
    // playerの中央
    const double playerCenter_X = player.getEntityCenter_X();

    // プレイヤーを画面中央付近へ維持する
    ctx.camera.x = playerCenter_X - (ctx.camera.width / 2.0);

    // WorldInfo.WorldWidthでクランプする
    const double maxCamera_X = std::max(0.0, (ctx.worldInfo.WorldWidth - ctx.camera.width));
    ctx.camera.x = std::clamp(ctx.camera.x, 0.0, maxCamera_X);
}

/**
 * @brief PlayingSceneへ入った際の初期化処理
 * 
 */
void PlayingScene::onEnter(){
    int stageIndex = ctrl.getCurrentStageIndex();
    ctrl.loadStage(stageIndex, ctx);
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
 * @brief 描画するオブジェクトの更新処理※Player/Enemyなど
 * 
 */
void PlayingScene::updateEntities(double delta, DrawBounds b){
    // キーの状態取得
    const InputState& is = ctx.input.getState();
    // ファイアボール
    for(auto& f : ctx.entityCtx.fireballs) f->update(delta, ctx.entityCtx.blocks);
    // キャラクタの更新
    // Player
    ctx.entityCtx.player.update(delta, is, b, ctx.entityCtx.blocks);
    // Enemy：通常の更新処理
    for(auto& e : ctx.entityCtx.enemies) e->update(delta, is, b, ctx.entityCtx.blocks);
    // Dying演出が終了しているEnemyを消す
    // ※Dying中の敵を消す処理は，updateEntities上で完結させる
    auto& enemies = ctx.entityCtx.enemies;
    // remove_ifは条件を満たす要素を除いたコンテナの終端イテレータを返す
    auto first = std::remove_if( 
        enemies.begin(),
        enemies.end(),
        [](const std::unique_ptr<Enemy>& e){
            return e->isDead();
        }
    );
    enemies.erase(first, enemies.end());
}

/**
 * @brief 描画するオブジェクトの衝突処理
 * 
 * 必ず呼び出す前にprevFeetCollisionをサンプリングしている必要がある
 */
void PlayingScene::detectCollision(){
    // 敵との衝突判定
    resolveEnemyCollision(ctx.entityCtx.player.getPrevFeetCollision());
    // ダメージブロックとの衝突判定
    resolveBlockCollision();
    // 敵とファイアボールの接触判定
    resolveFireBallEnemyCollision();
}

/**
 * @brief ブロックとの衝突処理用
 * 
 */
void PlayingScene::resolveBlockCollision(){
    // ダメージブロックとの衝突判定
    SDL_Rect playerRect = ctx.entityCtx.player.getCollisionRect();
    for(const auto& b : ctx.entityCtx.blocks){
        // ダメージ床, クリアオブジェクト以外は判定しない
        if(b.type != BlockType::Damage && b.type != BlockType::Clear){
            continue;   
        }
        SDL_Rect br = GameUtil::blockToRect(b);
        // 衝突しているかをまず判定(Rect)
        if(!GameUtil::intersects(playerRect, br)){
            continue;
        }
        if(b.type == BlockType::Damage){
            ctrl.requestScene(GameScene::GameOver);
        }else if(b.type == BlockType::Clear){
            // ステージ遷移
            ctrl.requestScene(GameScene::Clear);
        }
        return;
    }
}

/**
 * @brief 敵との衝突用の処理
 * 敵の踏みつけ/削除/スコア加算/GameOverの処理を行う
 * 
 */
void PlayingScene::resolveEnemyCollision(double prevFeet){
    // PlayerのRect取得
    auto& player = ctx.entityCtx.player;
    // プレイヤーのRect基準のあたり判定取得
    // ※update→Physics →clamp→getCollisionRectの順番が必要
    SDL_Rect playerRect = player.getCollisionRect();
    // 衝突処理用変数：collisionのfeetを使う
    // double prevFeet = player.getPrevFeetCollision();
    double newFeet  = player.getFeetCollision();
    double playerVv = player.getVerticalVelocity();
    
    // 接触のループ
    for(std::size_t i = 0; i < ctx.entityCtx.enemies.size(); ++i){
        auto& e = ctx.entityCtx.enemies[i];
        // 生きている敵のみ対象(Dying/Deadは除外)
        if(!e->isAlive()){
            continue;
        }
        // 矩形の当たり判定取得
        SDL_Rect enemyRect = e->getCollisionRect();
        // 接触判定
        auto result = PlayerEnemyCollision::resolvePlayerEnemyCollision(
            playerRect, 
            prevFeet, 
            newFeet, 
            playerVv, 
            enemyRect
        );

        // 接触判定結果で処理を分ける
        if(result == PlayerEnemyCollisionResult::None){
            continue;
        }
        if(result == PlayerEnemyCollisionResult::StompEnemy){
            // 敵を踏みつけ(後で敵を消す)
            e->startDying();    // Dying状態へ遷移
            // プレイヤーはバウンドする
            player.setVerticalVelocity(-std::abs(PlayerConfig::JUMP_VELOCITY));
            // スコア加算
            ctrl.setScore(ctrl.getScore() + EnemyConfig::SCORE_AT_STOMP);
            continue;   // 同フレーム中に同一の敵を二度ふまないようにcountinue
        }
        if(result == PlayerEnemyCollisionResult::PlayerHit){
            ctrl.requestScene(GameScene::GameOver);
            return;
        }
    }
}

/**
 * @brief 落下死判定関数
 * 
 */
void PlayingScene::hasFallenToGameOver(){
    //double death_Y = GameConfig::WINDOW_HEIGHT + PlayerConfig::FRAME_H; // 画面外へ落下死たら終了
    double death_Y = ctx.worldInfo.WorldHeight + PlayerConfig::FRAME_H; // 規定したworldInfoを使う
    double playerBottom = ctx.entityCtx.player.getCollisionRect().y
                          + ctx.entityCtx.player.getCollisionRect().h;
    if (playerBottom > death_Y){
        ctrl.requestScene(GameScene::GameOver);
        return;
    }
}

/**
 * @brief ファイアボールの画面外判定を返す
 * 
 * @return true 
 * @return false 
 */
bool PlayingScene::checkBoundsforFireBalls(SDL_Rect fr, const double world_W, const double world_H){
    // ファイアボールのマージン
    const double margin_H = FireBallConfig::FRAME_H;
    const double margin_W = FireBallConfig::FRAME_W;
    
    // ファイアボール端の座標を取得
    double top    = fr.y;
    double bottom = fr.y + fr.h;
    double right  = fr.x + fr.w;
    double left   = fr.x;

    // ワールド外へ出たFireBallは消す
    // 上下左右判定
    if(bottom < 0.0 - margin_H)       return true;    // 上に消えた
    if(top    > world_H + margin_H)   return true;    // 下に消えた
    if(right  < 0.0 - margin_W)       return true;    // 左に消えた
    if(left   > world_W + margin_W)   return true;    // 右に消えた

    return false;
}

/**
 * @brief 画面外へ出た/非活性になったファイアボールを消す
 * 
 */
void PlayingScene::cleanupFireBalls(){
    // ファイアボール取得
    auto& fireballs = ctx.entityCtx.fireballs;

    // それぞれのファイアボールの状態を確認して片付ける
    // 条件を満たすファイアボールを除いた配列を取得
    auto it = std::remove_if(
        fireballs.begin(), 
        fireballs.end(), 
        [&](const std::unique_ptr<FireBall>& f){
            if(!f->isActive()){
                return true;
            }
            SDL_Rect fr = f->getCollisionRect();
            return checkBoundsforFireBalls(fr, ctx.worldInfo.WorldWidth, ctx.worldInfo.WorldHeight);
        }
    );
    // remove_ifで消える要素はイテレータ範囲外へ動くのでeraseで消える
    fireballs.erase(it, fireballs.end());
}

/**
 * @brief 敵とファイアボールの接触判定処理
 * 
 */
void PlayingScene::resolveFireBallEnemyCollision(){
    // ファイアボール取得
    auto& fireballs = ctx.entityCtx.fireballs;
    // 敵を取得
    auto& enemies   = ctx.entityCtx.enemies;

    // それぞれの接触判定
    for(auto& f : fireballs){
        // 非活性のものは処理しない
        if(!f->isActive()){
            continue;
        }
        // 接触判定用矩形取得
        SDL_Rect fbr = f->getCollisionRect();
        // 敵との判定処理
        for(auto& e : enemies){
            // 非活性の敵は処理しない
            if(!e->isAlive()){
                continue;
            }
            // 接触判定用矩形取得
            SDL_Rect er = e->getCollisionRect();
            // 接触判定
            if(!GameUtil::intersects(fbr, er)){
                continue;   // 接触していないのは無視
            }
            // 命中時の処理
            e->startDying();    // 敵はDying状態へ
            f->deactivate();      // ファイアボールは非活性化
            ctrl.setScore(ctrl.getScore() + EnemyConfig::SCORE_AT_FIREBALL);   // 敵削除時のスコア加算
            break;  // ファイアボールは1体にヒットしたら終わる(貫通しない)
        }
    }
}
