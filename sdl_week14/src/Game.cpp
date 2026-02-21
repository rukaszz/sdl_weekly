// 定数
#include "GameConfig.hpp"
#include "PlayerConfig.hpp"
#include "EnemyConfig.hpp"

#include "SdlSystem.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "TitleScene.hpp"
#include "PlayingScene.hpp"
#include "GameOverScene.hpp"
#include "ClearScene.hpp"
#include "ResultScene.hpp"
#include "Texture.hpp"
#include "Character.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "FireBall.hpp"
#include "Game.hpp"
#include "GameUtil.hpp"
#include "Text.hpp"
#include "TextTexture.hpp"
#include "Input.hpp"
#include "BlockLevelLoader.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>

/**
 * @brief Construct a new Game:: Game object
 * 
 */
Game::Game(){
    // 初期化
    sdl = std::make_unique<SdlSystem>();
    bootstrapWindowAndRenderer();
    loadResources();
    buildWorld();
    buildContexts();
    buildScenes();
    startFromTitle();
}

/**
 * @brief Destroy the Game:: Game object
 * 
 */
Game::~Game(){

}

/**
 * @brief windowとrendererを作成する初期処理
 * 
 */
void Game::bootstrapWindowAndRenderer(){
    // ウィンドウ
    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    // レンダラー
    renderer = std::make_unique<Renderer>(window->get());
}

/**
 * @brief プレイヤー/フォントなどのテクスチャを読み込む
 * 
 */
void Game::loadResources(){
    // プレイヤー
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    // 敵
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    // ファイアボール
    fireballTexture = std::make_unique<Texture>(renderer->get(), "assets/image/fireball.png");
    // 敵の弾(現状はファイアボールと同じ)
    enemyBulletTexture = std::make_unique<Texture>(renderer->get(), "assets/image/fireball.png");
    // テキスト
    font = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    // スコア
    scoreText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    scoreText->setText("Score: 0");
    // FPS
    fpsText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    fpsText->setText("");
}

/**
 * @brief ゲーム部分の構成
 * 
 * プレイヤーやカメラなどを定義
 * 注意：worldInfoはloadStage()内で上書きされる(ここではウィンドウサイズで定義)
 */
void Game::buildWorld(){
    player = std::make_unique<Player>(*playerTexture);
    player->setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 世界の広さ
    worldInfo = {static_cast<double>(GameConfig::WINDOW_WIDTH), 
                 static_cast<double>(GameConfig::WINDOW_HEIGHT)
                };
    // 乱数
    distX = std::uniform_real_distribution<double>(GameConfig::WINDOW_WIDTH/2, GameConfig::WINDOW_WIDTH - EnemyConfig::FRAME_W);
    distY = std::uniform_real_distribution<double>(EnemyConfig::FRAME_H, GameConfig::WINDOW_HEIGHT - EnemyConfig::FRAME_H);
    distSpeed = std::uniform_real_distribution<double>(EnemyConfig::SPEED_MIN, EnemyConfig::SPEED_MAX);
    // カメラ
    camera = {0.0, 0.0, 
              static_cast<double>(GameConfig::WINDOW_WIDTH), 
              static_cast<double>(GameConfig::WINDOW_HEIGHT)
            };
}

/**
 * @brief GameContextを作成する
 * 
 * enemies/blocksはからのvectorなので注意
 */
void Game::buildContexts(){
    // input抽象化
    input = std::make_unique<Input>();
    // シーン
    ctx = std::make_unique<GameContext>(GameContext{
        *renderer,
        *input, 
        camera,
        worldInfo, 
        EntityContext{
            *player,
            *enemyTexture,
            enemies,    // 空vector
            blocks,     // 空vector
            *fireballTexture, 
            fireballs,  // 空vector
            *enemyBulletTexture, 
            enemyBullets   // 空vector
        }, 
        TextRenderContext{
            *font,
            *scoreText,
            *fpsText,
        }, 
        RandomContext{
            rd,
            distX,
            distY,
            distSpeed,
        }
    });
}

/**
 * @brief 各種のシーンをオブジェクト化
 * 
 */
void Game::buildScenes(){
    scenes[nextSceneIndex(GameScene::Title)]    = std::make_unique<TitleScene>(*this, *ctx);
    scenes[nextSceneIndex(GameScene::Playing)]  = std::make_unique<PlayingScene>(*this, *ctx);
    scenes[nextSceneIndex(GameScene::GameOver)] = std::make_unique<GameOverScene>(*this, *ctx);
    scenes[nextSceneIndex(GameScene::Clear)]    = std::make_unique<ClearScene>(*this, *ctx);
    scenes[nextSceneIndex(GameScene::Result)]   = std::make_unique<ResultScene>(*this, *ctx);
}

/**
 * @brief Gameコンストラクタ上で呼ばれ，タイトルシーンを読み込む
 * 
 */
void Game::startFromTitle(){
    // タイトルからスタート
    currentScene = scenes[nextSceneIndex(GameScene::Title)].get();
    currentScene->onEnter();
}

/**
 * @brief シーンの切り替えを実施する関数
 * 抽象クラスSceneに対して，ポリモーフィズムによる派生クラス生成を実施する
 * SceneControlでprotected
 * ※シーン変更処理のヘルパとして残すが，将来的には消える
 * 
 * @param id 
 */
void Game::changeScene(GameScene id){
    currentScene->onExit();
    currentScene = scenes[(int)id].get();
    currentScene->onEnter();
}

/**
 * @brief 各シーンで呼ばれ，次のシーンへの遷移をリクエストする
 * ※リクエストするだけで遷移はしない
 * 
 * @param id 
 */
void Game::requestScene(GameScene id){
    pendingSceneChange = id;
}

/**
 * @brief requestSceneを適用する
 * 
 * Game::update()内で呼ばれ，更新処理上でシーン遷移のリクエストがあればそれを適用する
 * 要求は複数回許可されるが，最後のリクエストが認められる
 */
void Game::applySceneChangeIfAny(){
    // 要求がなければ何もしない
    if(!pendingSceneChange){
        return;
    }
    // 現在のシーンから脱する
    if(currentScene){
        currentScene->onExit();
    }
    // シーン遷移を行う
    currentScene = scenes[nextSceneIndex(*pendingSceneChange)].get();
    currentScene->onEnter();
    // シーンリクエストのキューをクリア
    pendingSceneChange.reset();
}

/**
 * @brief ゲームの状態をリセットする関数
 * score/cameraをリセット
 * 
 * SceneControlが管理するので，タイトルからのNewGameのみ呼ぶ
 * 
 */
void Game::resetGame(){
    score = 0;
    camera.x = 0.0;
    camera.y = 0.0;
}

/**
 * @brief ゲーム実行用関数
 * 
 */
void Game::run(){
    // プラットフォームの最小間隔のクロック
    using clock = std::chrono::steady_clock;    // is_steadyな時刻を採用
    // 60Hz
    const double fixedDelta = 1.0 / 60.0;
    // ナノ秒単位での60fps間隔
    const auto fixedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(fixedDelta));
    // ループ開始前の時刻
    auto prev = clock::now();
    // 累積値
    std::chrono::nanoseconds acc{0};
    
    // FPS 計測用(これはミリ秒で十分)
    Uint32 fpsTimer = SDL_GetTicks();
    int fpsCounter = 0;

    while(running){
        // 時間の計測
        // フレーム開始点
        Uint32 frameStartMs = SDL_GetTicks();
        // 入力値の受け取りはなるべく最初に実施する
        // 入力受付
        processEvents();
        // input->beginFrame();
        // 時間計測と更新
        auto now = clock::now();
        acc += std::chrono::duration_cast<std::chrono::nanoseconds>(now - prev);
        prev = now;

        // accの大量累積によるゲーム大幅更新の防止
        // 更新が動かず処理が累積しても，最大で5回しかupdate()を実施しない
        // ※遅延蓄積時でもprocessEventsは1回のみ呼ばれることに留意
        int safety = 0;
        while(acc >= fixedNs && safety++ < 5){
            update(fixedDelta);
            acc -= fixedNs;
        }
        // シーンの更新
        applySceneChangeIfAny();
        // 補間係数(描画用)
        // const double alpha = std::clamp(acc.count() / static_cast<double>(fixedNs.count()), 0.0, 1.0);
        // 描画
        render();
        
        // input->endFrame();
        // update(delta);
        // render();
        // フレームレートの計算とfps計測
        ++fpsCounter;
        Uint32 nowMs = SDL_GetTicks();
        if(nowMs - fpsTimer >= 1000){
            fpsText->setText("FPS: " + std::to_string(fpsCounter));
            fpsCounter = 0;
            fpsTimer = nowMs;
        }
        // fpsキャップ(最大60fps)
        capFrameRate(frameStartMs);
    }
}

/**
 * @brief fpsキャップを実装(fpsを固定化するため)
 * 
 * @param frameStartMs: 計測点
 * @return * void 
 */
void Game::capFrameRate(Uint32 frameStartMs){
    // 16.6ms - 経過時間の概算
    Uint32 frameDuration = SDL_GetTicks() - frameStartMs;
    if(frameDuration < Game::FRAME_DELAY){
        // スリープ処理
        Uint32 sleepMs = Game::FRAME_DELAY - frameDuration;
        if(sleepMs > 1){
            // 余り2ms以上でSDL_Delayを呼び出す
            SDL_Delay(sleepMs - 1);
            // 最後の1ms弱はbusy waitで微調整する(高負荷なので最小限)
            while ((SDL_GetTicks() - frameStartMs) < Game::FRAME_DELAY){
                /* spin */
            }
            
        }
    }
}

/**
 * @brief ポーリング処理によるイベント管理
 * 
 */
void Game::processEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            running = false;
        }
        input->handleEvent(e);
        #ifdef DEBUG_INPUT
        input->debugPrintInput();
        #endif  // DEBUG_INPUT
        currentScene->handleEvent(e);
    }
}

/**
 * @brief ゲーム更新関数
 * 
 * @param delta: 前フレームとの差分
 */
void Game::update(double delta){
    // input->update(); // ここでupdadeするとキー入力がSceneへ伝搬しない
    currentScene->update(delta);
    input->update();
}

/**
 * @brief 描画の更新用
 * 
 */
void Game::render(){
    renderer->clear();
    currentScene->render();
    renderer->present();
}
