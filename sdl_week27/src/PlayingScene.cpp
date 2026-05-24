#include "PlayingScene.hpp"

#include <algorithm>
#include <cmath>
#include <variant>
#include <vector>

#include <SDL2/SDL.h>

#include "Camera.hpp"
#include "CameraShakeController.hpp"
#include "PlayerEnemyCollisionUtil.hpp"
#include "Renderer.hpp"
#include "Game.hpp"
#include "GameUtil.hpp"
#include "MusicId.hpp"
#include "BackgroundRenderer.hpp"
#include "BackgroundPresetBuilder.hpp"
#include "BossBattleSystem.hpp"

#include "BossEnemy.hpp"
#include "TurretEnemy.hpp"

// 設定読み込み
#include "GameConfig.hpp"
#include "EnemyConfig.hpp"
#include "BossConfig.hpp"
#include "FireBallConfig.hpp"
#include "EnemyBulletConfig.hpp"
#include "UIConfig.hpp"
#include "BossBattleState.hpp"

/**
 * @brief Construct a new Playing Scene:: Playing Scene object
 * 
 */
PlayingScene::PlayingScene(SceneControl& sc, GameContext& gc)
    : Scene(sc, gc)
    , bgRenderer(
        GameConfig::WINDOW_WIDTH, 
        GameConfig::WINDOW_HEIGHT
    ), projectiles(
        ctx.entityCtx.fireballs, 
        ctx.entityCtx.enemyBullets, 
        ctx.entityCtx.blocks, 
        ctx.worldInfo, 
        ctx.entityCtx.fireballTexture, 
        ctx.entityCtx.enemyBulletTexture
    ), enemyAI(
        ctx.entityCtx.enemies, 
        ctx.entityCtx.blocks, 
        ctx.entityCtx.blockRectCaches, 
        ctx.worldInfo, 
        ctx.entityCtx.player
    ), bossBattleSystem(
        ctx.entityCtx.boss,
        ctx.entityCtx.player,
        ctx.entityCtx.blocks,
        ctx.entityCtx.blockRectCaches,
        ctx.worldInfo
    ), collision(
        ctx.entityCtx.player, 
        ctx.entityCtx.enemies, 
        ctx.entityCtx.blocks, 
        ctx.entityCtx.blockRectCaches, 
        ctx.worldInfo
    ), blockSystem(
        ctx.entityCtx.blocks, 
        ctx.entityCtx.player
    ), items(
        ctx.entityCtx.items
    ), playerState(
        ctx.entityCtx.player
    ), itemRenderer(
        ctx.entityCtx.items, 
        ctx.renderAssets.itemTextures
    ), blockRenderer(
        ctx.entityCtx.blocks, 
        ctx.entityCtx.blockRectCaches, 
        ctx.renderAssets.blockTextures
    )
{
    // ポーズ時の文字列
    pauseTitleText  = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 100, 255});
    pauseTitleText->setText("PAUSE");
    gameResumeText  = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    gameResumeText->setText("Press ESC to Resume");
    backToTitleText = std::make_unique<TextTexture>(ctx.renderer, ctx.textRenderCtx.font, SDL_Color{255, 255, 255, 255});
    backToTitleText->setText("Press BackSpace to Title");
    // デバッグ用文字列
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
 * 呼び出し順序は記載の方法で実施すること
 * パイプライン方式で処理の結果が伝搬していく
 * 
 * @param delta 
 */
void PlayingScene::update(double delta){
    // 1. 入力の受け入れ
    // エスケープキーでTitleへの遷移
    const InputState& is = ctx.input.getState();
    handlePlayingInput(is);
    if(runState == RunState::Paused){
        return; // ポーズ中はすべての更新を止める
    }
    // プレイヤー死亡時は死亡演出系のみ更新する
    if(runState == RunState::PlayerDying){
        updatePlayerDying(delta);
        return;
    }
    // 2. 衝突処理用の前フレームのプレイヤー座標をサンプリング
    // 必ず各種Collision判定前に呼ぶ必要がある
    // 呼び出し順序に注意すること
    ctx.entityCtx.player.beginFrameCollisionSample();
    // ボス戦開始点を超えたか判定
    bossBattleSystem.updateTrigger(ctx.musicSystem, ctx.events);
    // 3. worldInfoを用いた幅のクランプ処理
    DrawBounds worldBounds = {
        .min_X = 0.0, 
        .min_Y = 0.0, 
        .max_X = ctx.worldInfo.WorldWidth, 
        .max_Y = ctx.worldInfo.WorldHeight
    };
    // ボス戦では制限した範囲内にPlayerを閉じ込める
    if(bossBattleSystem.isActive()){
        worldBounds.min_X = bossBattleSystem.getCameraMin_X();
        worldBounds.max_X = bossBattleSystem.getCameraMax_X();
    }
    // 4. スコア更新
    updateScore(delta);
    // 5. 敵センサの収集とAIの更新
    enemyAI.update(delta);
    bossBattleSystem.updateAI(delta);
    // 6. 物理の更新(弾系はここで更新していない)
    updateEntities(delta, worldBounds);
    // Playerがこのフレームでジャンプしたかチェック
    if(ctx.entityCtx.player.consumeJumpStartedThisFrame()){
        ctx.events.playSound(SoundId::Jump);
    }
    // 7. ブロックの殴打処理
    // 下からの殴打
    collision.resolveBlockHits(ctx.events);
    // ブロックに関するイベント
    blockSystem.process(ctx.eventBuffer);
    // 8. ブロック殴打後のアイテム処理
    // 取ったのに死んだ，とならないように先に処理する
    items.processSpawn(ctx.eventBuffer);
    items.resolvePlayerCollision(ctx.entityCtx.player, ctx.eventBuffer);
    playerState.process(ctx.eventBuffer);
    // アイテム取得によるプレイヤーの状態遷移を即時反映させる
    // ここで遷移できない場合は，次フレームのplayer.update()の冒頭で遷移をチェックする
    ctx.entityCtx.player.flushPendingFormChange(ctx.entityCtx.blocks);
    // 9. 弾(projectile)の処理
    // ボス戦状態ならここで処理する
    if(bossBattleSystem.isActive()){
        bossBattleSystem.updateBoss(delta, worldBounds, is);
        projectiles.spawnBossBullets(ctx.entityCtx.boss);
    }
    // 敵弾生成(Turretへの射出要求を消費)
    projectiles.spawnEnemyBulletsFromEnemies(ctx.entityCtx.enemies);
    // 弾更新(プレイヤー弾/敵弾で統一)
    projectiles.update(delta);
    // 10. Playerとの当たり判定
    // ブロック
    // イベント発行用のIGameEventsを使う
    // Clear/Damageブロックの処理
    collision.resolveSpecialBlockCollision(ctx.events);
    // 敵との接触
    collision.resolveEnemyCollision(ctx.events);
    // ボス用の接触※isAliveのガードは処理内部で実施
    bossBattleSystem.resolveBossPlayerCollision(ctx.entityCtx.player, ctx.events);
    // 弾の当たり判定は System に移す(detectCollisionから除外している)
    projectiles.resolveCollisions(
        ctx.entityCtx.player,
        ctx.entityCtx.enemies,
        ctx.entityCtx.boss,
        bossBattleSystem.isActive(),
        ctx.events
    );
    // 11. 落下死判定
    collision.checkFallDeath(ctx.events);    
    // 12. ボス戦トリガーの監視
    bossBattleSystem.checkBattleResult(ctx.events);
    // 13. Scene内のイベント消費
    consumeShakeEffectEvents();
    consumePlayerDeathEvents(); // プレイヤー死亡時のパーティクルを出したいのでParticleイベント消費より先に処理する
    consumeParticleEvents();
    // 14. カメラ座標の更新
    updateCamera();
    // 画面シェイク
    cameraShake.update(delta, ctx.randomCtx.random);
    // 15. パーティクルの更新
    particles.update(delta);
    // 16. 消費系オブジェクトの片付け
    projectiles.cleanup();
    items.cleanup();
    // デバッグ情報取得
    debugText->setText(ctx.entityCtx.player.debugMoveContext());
}

/**
 * @brief ゲーム中の画面描画処理
 * 
 */
void PlayingScene::render(){
    /* 画面シェイク用カメラ */
    // 画面シェイクを適用する場合はshaken, 適用しないならctx.camera
    Camera shaken = cameraShake.applyToCamera(ctx.camera);
    // 背景
    bgRenderer.renderBackground(ctx.renderer, shaken);      // 最初は背景用画像の描画
    bgRenderer.renderBgDecoration(ctx.renderer, shaken);    // 次に背景用装飾の描画
    // ブロック描画
    blockRenderer.render(ctx.renderer, shaken);
    // アイテム描画
    itemRenderer.render(ctx.renderer, shaken);
    // プレイヤーのファイアボール描画
    for(const auto& f : ctx.entityCtx.fireballs){
        f->draw(ctx.renderer, shaken);
    }
    // 敵弾描画
    for(const auto& eb : ctx.entityCtx.enemyBullets){
        eb->draw(ctx.renderer, shaken);
    }
    // キャラクタ描画
    // カメラを考慮した書き方にする
    // プレイヤー死亡演出時ではないとき or (プレイヤー死亡演出時は)プレイヤーの描画可能時
    if(runState != RunState::PlayerDying || deathEvent.isVisible()){
        // プレイヤーの描画
        ctx.entityCtx.player.draw(ctx.renderer, shaken);
    }
    for(auto& e : ctx.entityCtx.enemies){
       e->draw(ctx.renderer, shaken);
    }
    if(bossBattleSystem.isActive()){
        ctx.entityCtx.boss.draw(ctx.renderer, shaken);
        // renderBossHpBar();  // 注：Dyingでも表示させる
        bossBattleSystem.renderHpBar(ctx.renderer, ctx.camera);
    }
    // パーティクルの描画
    particles.render(ctx.renderer, shaken);
    // デバッグ情報表示
    debugText->draw(ctx.renderer, 20, 80);
    // テキスト描画
    ctx.textRenderCtx.fpsText.draw(ctx.renderer, 20, 20);
    // スコア更新
    ctx.textRenderCtx.scoreText.setText(
        "Score: " + std::to_string(static_cast<int>(ctrl.getScore()))
    );
    ctx.textRenderCtx.scoreText.draw(ctx.renderer, 20, 50);   
    // ポーズ中も描画はするので，最後に薄暗いオーバーレイをする
    if(runState == RunState::Paused){
        renderPauseOverlay();
        return;
    }
}

/**
 * @brief PlayingSceneへ入った際の初期化処理
 * 
 */
void PlayingScene::onEnter(){
    // 現在のステージインデックスを取得してステージをロード
    int stageIndex = ctrl.getCurrentStageIndex();
    // runStateの初期化
    runState = RunState::Running;
    // カメラ関係の変数リセット
    cameraShake.reset();
    // stageIndexに応じた定義の読み込み
    ctrl.loadStage(stageIndex, ctx);
    // ブロックのキャッシュ再構築
    GameUtil::rebuildBlockRects(ctx.entityCtx.blocks, ctx.entityCtx.blockRectCaches);
    // 背景の読み込み
    loadBackground();
    // ステージが変わった通知を各システムへ送信する
    items.onStageLoaded();
    projectiles.onStageLoaded();
    enemyAI.onStageLoaded();
    collision.onStageLoaded();
    particles.clear();
    deathEvent.reset();
    // ボス関係の変数初期化
    // initBossBattle();
    const auto& def = ctrl.getCurrentStageDefinition();
    bossBattleSystem.init(def);
    // BGM再生
    ctx.musicSystem.playIfChanged(MusicId::Playing);
}

/**
 * @brief PlayingSceneから別シーンへ移る際の終了処理
 * 
 */
void PlayingScene::onExit(){
    // PlayingSceneを出るときもrunStateは初期化
    runState = RunState::Running;
    // カメラ関係の変数リセット
    cameraShake.reset();
    // パーティクルが残らないようリセット
    particles.clear();
    // プレイヤー死亡演出リセット
    deathEvent.reset();
}

/**
 * @brief 入力の処理
 * 
 */
void PlayingScene::handlePlayingInput(const InputState& is){
    // 死亡演出時は各種の入力を受け付けない
    if(runState == RunState::PlayerDying){
        return;
    }
    // ポーズ中の処理
    if(runState == RunState::Paused){
        // Escキー
        if(is.justPressed[static_cast<int>(Action::Pause)]){
            // ゲーム再開
            ctx.events.playSound(SoundId::PauseClose);
            runState = RunState::Running;
            ctx.musicSystem.resume();
        }
        // BackSpaceキー
        if(is.justPressed[static_cast<int>(Action::BackSpace)]){
            // BackSpaceキーでタイトルへ
            ctx.events.requestScene(GameScene::Title);
            return;
        }
        // ポーズ中は他の入力を無視する
        return;
    }
    // Escキー
    if(is.justPressed[static_cast<int>(Action::Pause)]){
        ctx.events.playSound(SoundId::PauseOpen);
        runState = RunState::Paused;
        ctx.musicSystem.pause();
        return;
    }
    // bキーでファイアボール発射
    if(is.justPressed[static_cast<int>(Action::Fire)]){
        // 出現位置と向きの設定
        if(ctx.entityCtx.player.getForm() == PlayerForm::Fire){
            const double spawn_X = ctx.entityCtx.player.getEntityCenter_X();
            const double spawn_Y = ctx.entityCtx.player.getEntityCenter_Y();
            const Direction dir = ctx.entityCtx.player.getDirection();
            // projectileでファイアボールを管理(戻り値がboolなので使う)
            if(projectiles.trySpawnPlayerFireball(spawn_X, spawn_Y, dir)){
                // ファイアボール発射音
                ctx.events.playSound(SoundId::Fireball);
            }
        }
    }
}

/**
 * @brief スコア算出処理
 * 
 */
void PlayingScene::updateScore(double delta){
    // スコア計算
    survivalScoreRemainder += delta * GameConfig::SCORE_RATE;
    const int wholeScore = static_cast<int>(survivalScoreRemainder); // 端数を切り捨ててスコアとする
    if(wholeScore > 0){
        ctx.events.addScore(wholeScore);
        survivalScoreRemainder -= wholeScore;
    }
}

/**
 * @brief 描画するオブジェクトの更新処理※Player/Enemyなど
 * 
 */
void PlayingScene::updateEntities(double delta, DrawBounds b){
    // キーの状態取得
    const InputState& is = ctx.input.getState();
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

    // カメラ追従の補正
    if(bossBattleSystem.isActive()){
        // ボス戦の場合はボス戦に向けた補正をする
        const double bossCameraMaxLeft = bossBattleSystem.getCameraMax_X() - ctx.camera.width;
        ctx.camera.x = std::clamp(ctx.camera.x, bossBattleSystem.getCameraMin_X(), bossCameraMaxLeft);
    } else {
        // WorldInfo.WorldWidthでクランプする
        const double maxCamera_X = std::max(0.0, (ctx.worldInfo.WorldWidth - ctx.camera.width));
        ctx.camera.x = std::clamp(ctx.camera.x, 0.0, maxCamera_X);
    }
    
}

/**
 * @brief ポーズ時の画面表示
 * 薄暗くするレイヤと文字を表示する 
 * 
 */
void PlayingScene::renderPauseOverlay(){
    // 半透明の黒のオーバーレイのサイズ設定
    SDL_Point outputSize = ctx.renderer.getOutputSize();
    SDL_Rect overlay{0, 0, outputSize.x, outputSize.y};
    SDL_Color overlayColor{0, 0, 0, 160};
    ctx.renderer.drawTranslucentOverlay(overlay, overlayColor);
    // テキスト描画
    // "PAUSE" テキスト（中央上部）
    pauseTitleText->draw(
        ctx.renderer,
        outputSize.x/2 - pauseTitleText->getWidth()/2,
        outputSize.y/3 - pauseTitleText->getHeight()/2
    );
    // "Press ESC to Resume"（中央）
    gameResumeText->draw(
        ctx.renderer,
        outputSize.x/2 - gameResumeText->getWidth()/2,
        outputSize.y/2 - gameResumeText->getHeight()/2
    );
    // "Press BackSpase to Title"（中央下）
    backToTitleText->draw(
        ctx.renderer,
        outputSize.x/2 - backToTitleText->getWidth()/2,
        outputSize.y/1.5 - backToTitleText->getHeight()/2
    );
}

/**
 * @brief ステージ開始時(onEnter時)に背景を読み込む処理
 * 
 */
void PlayingScene::loadBackground(){
    BackgroundPresetBuilder::build(
        bgRenderer, 
        ctx.renderAssets.bgTextures, 
        ctrl.getCurrentStageDefinition()
    );
}

/**
 * @brief 画面シェイクイベントの消費
 * イベントバッファに存在する画面シェイクイベントを消費し，画面シェイクを実施する
 * 
 */
void PlayingScene::consumeShakeEffectEvents(){
    // バッファを走査
    ctx.eventBuffer.consumeIf(
        // StartCameraShakeEvents型があるかを判定(holds_alternative)
        [](const GameEvent& ev){
            return std::holds_alternative<StartCameraShakeEvent>(ev);
        }, 
        // StartCameraShakeEventsを取り出して画面シェイクAPIを呼び出す
        [&](const GameEvent& ev){
            const auto& cse = std::get<StartCameraShakeEvent>(ev);
            cameraShake.start(cse.duration, cse.magnitude);
        }
    );
}

/**
 * @brief パーティクルイベントの消費
 * イベントバッファに存在するパーティクル発生イベントを消費してパーティクルを発生させる
 * 
 */
void PlayingScene::consumeParticleEvents(){
    // バッファを走査
    ctx.eventBuffer.consumeIf(
        // SpawnParticleEvent型があるかを判定(holds_alternative)
        [](const GameEvent& ev){
            return std::holds_alternative<SpawnParticleEvent>(ev);
        }, 
        // SpawnParticleEventを取り出す
        [&](const GameEvent& ev){
            const auto& spe = std::get<SpawnParticleEvent>(ev);
            // idを確認して対応するパーティクルを出現させる
            switch(spe.id){
            case ParticleEffectId::CoinSpark:
                particles.spawnCoinSpark(spe.x, spe.y);
                break;
            case ParticleEffectId::EnemyBurst:
                particles.spawnEnemyBurst(spe.x, spe.y);
                break;
            case ParticleEffectId::PlayerDeath:
                particles.spawnPlayerDeath(spe.x, spe.y);
                break;
            case ParticleEffectId::BlockDebri:
                particles.spawnBlockDebri(spe.x, spe.y);
                break;
            }
        }
    );
}

/**
 * @brief プレイヤー死亡イベントの消費
 * イベントバッファに存在するrequestPlayerDeathEventを消費する
 * 
 */
void PlayingScene::consumePlayerDeathEvents(){
    // バッファを走査
    ctx.eventBuffer.consumeIf(
        // RequestPlayerDeathEvent型があるかを判定(holds_alternative)
        [](const GameEvent& ev){
            return std::holds_alternative<RequestPlayerDeathEvent>(ev);
        }, 
        // SpawnParticleEventを取り出して画面シェイクAPIを呼び出す
        [&](const GameEvent& ev){
            if(runState == RunState::PlayerDying){
                return; // 二重発火の防止
            }
            // イベント取り出し
            const auto& pde = std::get<RequestPlayerDeathEvent>(ev);
            // runStateの変更
            runState = RunState::PlayerDying;
            // プレイヤー死亡演出開始
            deathEvent.start();
            // BGMのフェードアウト(1000ms)
            ctx.musicSystem.fadeOut(1000);
            // パーティクルも呼び出す(この時点でプレイヤーの死亡は確定しているので)
            ctx.events.spawnParticle(
                ParticleEffectId::PlayerDeath, 
                pde.x, 
                pde.y
            );
        }
    );
}

/**
 * @brief プレイヤー死亡演出時の更新
 * プレイヤー死亡時はパーティクル・カメラシェイク処理のみ更新する
 * 
 * @param delta 
 */
void PlayingScene::updatePlayerDying(double delta){
    // パーティクル
    particles.update(delta);
    // カメラシェイク
    cameraShake.update(delta, ctx.randomCtx.random);
    // 死亡演出のタイマーを更新し，終了後GameOverへ遷移(タイマーが0になるとtrueが返る)
    if(deathEvent.update(delta)){
        ctx.events.requestScene(GameScene::GameOver);
    }
}
