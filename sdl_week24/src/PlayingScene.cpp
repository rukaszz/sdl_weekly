#include "PlayingScene.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

#include <SDL2/SDL.h>

#include "PlayerEnemyCollisionUtil.hpp"
#include "Renderer.hpp"
#include "Game.hpp"
#include "GameUtil.hpp"
#include "MusicId.hpp"
#include "BackgroundRenderer.hpp"
#include "BackgroundPresetBuilder.hpp"

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
    ), bossAI(
        ctx.entityCtx.boss,
        ctx.entityCtx.player,
        ctx.entityCtx.blocks, 
        ctx.entityCtx.blockRectCaches, 
        ctx.worldInfo, 
        bossBattle
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
    // 2. 衝突処理用の前フレームのプレイヤー座標をサンプリング
    // 必ず各種Collision判定前に呼ぶ必要がある
    // 呼び出し順序に注意すること
    //ctx.entityCtx.player.beginFrameFeetCollisionSample();
    ctx.entityCtx.player.beginFrameCollisionSample();
    // ボス戦開始点を超えたか判定
    updateBossBattleTrigger();
    // 3. worldInfoを用いた幅のクランプ処理
    DrawBounds worldBounds = {
        .min_X = 0.0, 
        .min_Y = 0.0, 
        .max_X = ctx.worldInfo.WorldWidth, 
        .max_Y = ctx.worldInfo.WorldHeight
    };
    // ボス戦では制限した範囲内にPlayerを閉じ込める
    if(bossBattle.isActive()){
        worldBounds.min_X = bossBattle.cameraMin_X;
        worldBounds.max_X = bossBattle.cameraMax_X;
    }
    // 4. スコア更新
    updateScore(delta);
    // 5. 敵センサの収集とAIの更新
    enemyAI.update(delta);
    bossAI.update(delta);
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
    if(bossBattle.isActive()){
        updateBoss(delta, worldBounds);
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
    collision.resolveEnemyCollision(ctx.events);
    // 弾の当たり判定は System に移す(detectCollisionから除外している)
    projectiles.resolveCollisions(
        ctx.entityCtx.player, 
        ctx.entityCtx.enemies, 
        ctx.entityCtx.boss,
        bossBattle.isActive(), 
        ctx.events
    );
    // 11. 落下死判定
    collision.checkFallDeath(ctx.events);    
    // 12. ボス戦トリガーの監視
    updateBossBattleResult();

    // 13. カメラ座標の更新
    updateCamera();
    // 14. 消費系オブジェクトの片付け
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
    // 背景
    bgRenderer.renderBackground(ctx.renderer, ctx.camera);      // 最初はフルスクリーン画像 
    bgRenderer.renderBgDecoration(ctx.renderer, ctx.camera);    // 次に背景用装飾
    // ブロック描画
    blockRenderer.render(ctx.renderer, ctx.camera);
    // アイテム描画
    itemRenderer.render(ctx.renderer, ctx.camera);
    // プレイヤーのファイアボール描画
    for(const auto& f : ctx.entityCtx.fireballs){
        f->draw(ctx.renderer, ctx.camera);
    }
    // 敵弾描画
    for(const auto& eb : ctx.entityCtx.enemyBullets){
        eb->draw(ctx.renderer, ctx.camera);
    }
    // キャラクタ描画
    // カメラを考慮した書き方にする
    ctx.entityCtx.player.draw(ctx.renderer, ctx.camera);
    for(auto& e : ctx.entityCtx.enemies) e->draw(ctx.renderer, ctx.camera);
    if(bossBattle.isActive()){
        ctx.entityCtx.boss.draw(ctx.renderer, ctx.camera);
        renderBossHpBar();  // 注：Dyingでも表示させる
    }
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
    // ボス関係の変数初期化
    initBossBattle();
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
}

/**
 * @brief 入力の処理
 * 
 */
void PlayingScene::handlePlayingInput(const InputState& is){
    // ポーズ中の処理
    if(runState == RunState::Paused){
        // Escキー
        if(is.justPressed[(int)Action::Pause]){
            // ゲーム再開
            ctx.events.playSound(SoundId::PauseClose);
            runState = RunState::Running;
            ctx.musicSystem.resume();
        }
        // BackSpaceキー
        if(is.justPressed[(int)Action::BackSpace]){
            // BackSpaceキーでタイトルへ
            ctx.events.requestScene(GameScene::Title);
            return;
        }
        // ポーズ中は他の入力を無視する
        return;
    }
    // Escキー
    if(is.justPressed[(int)Action::Pause]){
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
    if(bossBattle.isActive()){
        // ボス戦の場合はボス戦に向けた補正をする
        const double bossCameraMaxLeft = bossBattle.cameraMax_X - ctx.camera.width;
        ctx.camera.x = std::clamp(ctx.camera.x, bossBattle.cameraMin_X, bossCameraMaxLeft);
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

// /**
//  * @brief loadBackgroundのヘルパ関数
//  * jsonに記載されたtypeに応じたテクスチャを選択する
//  * 
//  * @param type 
//  * @return const Texture& 
//  */
// const Texture& PlayingScene::selectDecorationTexture(BgDecorationType type) const{
//     switch (type){
//     case BgDecorationType::Cloud:
//         return ctx.renderAssets.bgTextures.cloudTexture;
//     case BgDecorationType::Star:
//         return ctx.renderAssets.bgTextures.starTexture;
//     case BgDecorationType::DarkSun:
//         return ctx.renderAssets.bgTextures.darkSunTexture;
//     default:
//         break;
//     }
// }

// /**
//  * @brief ステージ開始時(onEnter時)に背景を読み込む処理
//  * 
//  */
// void PlayingScene::loadBackground(){
//     // 背景読み込み
//     bgRenderer.clearLayers();
//     bgRenderer.clearDecoration();
//     // ステージ定義を確認
//     const auto& stageDef = ctrl.getCurrentStageDefinition();
//     switch(stageDef.backgroundId){
//     case BackgroundId::Forest:
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.sky, 0.0);      // 空：スクロールしない
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.mountain, 0.1); // 山：ゆっくりスクロール
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.forest, 0.3);   // 森：中速スクロール
//         // 雲をworld座標に個別配置　TODO：マジックナンバーは取り除くこと
//         for(const auto& bd : stageDef.bgDecorations){
//             const Texture& bgDecoTex = selectDecorationTexture(bd.type);
//             bgRenderer.addDecoration(bgDecoTex, bd.world_X, bd.screen_Y, bd.parallaxFactor);
//         }
//         break;
//     case BackgroundId::DarkForest:
//         // bgRenderer.addLayer(ctx.renderAssets.bgTextures.darkForest, 0.3);   // 黒い森：中速スクロール
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.darkSky, 0.0);      // 空：スクロールしない
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.darkMountain, 0.1); // 山：ゆっくりスクロール
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.darkForest, 0.3);   // 森：中速スクロール
//         // 装飾をworld座標に個別配置　TODO：マジックナンバーは取り除くこと
//         for(const auto& bd : stageDef.bgDecorations){
//             const Texture& bgDecoTex = selectDecorationTexture(bd.type);
//             bgRenderer.addDecoration(bgDecoTex, bd.world_X, bd.screen_Y, bd.parallaxFactor);
//         }
//         break;
//     case BackgroundId::HellForest:
//         // bgRenderer.addLayer(ctx.renderAssets.bgTextures.hellForest, 0.3);   // 赤黒い森：中速スクロール
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.hellSky, 0.0);      // 空：スクロールしない
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.hellMountain, 0.1); // 山：ゆっくりスクロール
//         bgRenderer.addLayer(ctx.renderAssets.bgTextures.hellForest, 0.3);   // 森：中速スクロール
//         // 装飾をworld座標に個別配置　TODO：マジックナンバーは取り除くこと
//         for(const auto& bd : stageDef.bgDecorations){
//             const Texture& bgDecoTex = selectDecorationTexture(bd.type);
//             bgRenderer.addDecoration(bgDecoTex, bd.world_X, bd.screen_Y, bd.parallaxFactor);
//         }
//         break;
//     }
// }

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

// ボス系処理
/**
 * @brief BossEnemyとBossBattleStateの初期化処理
 * 
 */
void PlayingScene::initBossBattle(){
    // StageDefinitionを取得
    const auto& def = ctrl.getCurrentStageDefinition();
    // BossBattleState
    bossBattle.phase          = def.bossBattleDef.enabled
        ? BossBattlePhase::WaitingTrigger : BossBattlePhase::None;
    bossBattle.trigger_X      = def.bossBattleDef.trigger_X;
    bossBattle.cameraMin_X    = def.bossBattleDef.cameraMin_X;
    bossBattle.cameraMax_X    = def.bossBattleDef.cameraMax_X;
    // ボスのスポーン位置などはloadStage()でやる
}

/**
 * @brief BossBattleStateの監視・更新関数
 * ボスが負けたかなどの状態をチェックする
 * 
 */
void PlayingScene::updateBossBattleTrigger(){
    // ボス戦が無いステージでは処理しない
    if(!bossBattle.isWaiting()){
        return;
    }
    // プレイヤーの中心座標
    const double player_X = ctx.entityCtx.player.getEntityCenter_X();
    if(player_X >= bossBattle.trigger_X){
        bossBattle.phase = BossBattlePhase::Active;
        // BGM再生
        ctx.musicSystem.playIfChanged(MusicId::Boss);
    }
}

/**
 * @brief ボス関係のデータの更新
 * 
 */
void PlayingScene::updateBoss(double delta, DrawBounds bounds){
    // boss, inputstate取得
    auto& boss = ctx.entityCtx.boss;
    const InputState& is = ctx.input.getState();

    boss.update(delta, is, bounds, ctx.entityCtx.blocks);
}

/**
 * @brief ボスに勝ったかどうかの判定を行う関数
 * 
 */
void PlayingScene::updateBossBattleResult(){
    if(!bossBattle.isActive()){
        return;
    }
    // ボスが死んだ際の処理
    if(!ctx.entityCtx.boss.isDead()){
        return;
    }
    bossBattle.phase = BossBattlePhase::Defeated;
    ctx.events.requestScene(GameScene::Clear);
}

/**
 * @brief ボスのHPバーの描画関数
 * render()で呼び出す
 * 
 */
void PlayingScene::renderBossHpBar(){
    // TODO：
    // 座標
    const int bar_W = UIConfig::BossHpBarConfig::BAR_W;
    const int bar_H = UIConfig::BossHpBarConfig::BAR_H;
    const int bar_X = static_cast<int>((ctx.camera.width - bar_W)/2.0); // 画面中央
    const int bar_Y = UIConfig::BossHpBarConfig::BAR_Y;
    // ボスのHP取得
    const int hp = ctx.entityCtx.boss.getHp();
    const int maxHp = ctx.entityCtx.boss.getMaxHp();
    if(maxHp <= 0){
        return;
    }
    // バーの比率
    double hpRatio = std::clamp(static_cast<double>(hp) / static_cast<double>(maxHp), 0.0, 1.0);
    // HPバーの幅を比率に合わせる
    const int filled_W = static_cast<int>(bar_W * hpRatio);
    // HPバーの矩形定義(Frameは少し大き目)
    const int offset = UIConfig::BossHpBarConfig::FRAME_OFFSET;
    const SDL_Rect frameRect = {bar_X-offset, bar_Y-offset, bar_W+(offset*2), bar_H+(offset*2)};
    const SDL_Rect bgRect   = {bar_X, bar_Y, bar_W, bar_H};
    const SDL_Rect fillRect   = {bar_X, bar_Y, filled_W, bar_H};
    // HPバーの矩形色定義
    // 枠
    const SDL_Color frameColor   = UIConfig::BossHpBarConfig::FRAME_COLOR;
    // 背景
    const SDL_Color bgColor      = UIConfig::BossHpBarConfig::BG_COLOR;
    // HPバーの色
    SDL_Color fillColor{};
    if(hpRatio > 0.6){
        // 緑
        fillColor = UIConfig::BossHpBarConfig::HIGH_HP_COLOR;
    } else if(hpRatio > 0.3){
        // 黄
        fillColor = UIConfig::BossHpBarConfig::MIDDLE_HP_COLOR;
    } else {
        // 赤
        fillColor = UIConfig::BossHpBarConfig::LOW_HP_COLOR;
    }
    // 描画：枠→背景→HPバーの順
    // 枠
    ctx.renderer.drawRect(frameRect, frameColor);
    // 背景
    ctx.renderer.drawRect(bgRect, bgColor);
    // HPバー
    ctx.renderer.drawRect(fillRect, fillColor);
}
