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
#include "Texture.hpp"
#include "Character.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
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

void Game::initSDL(){
    // SDL初期化
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        throw std::runtime_error(SDL_GetError());
    }
    // SDL_IMG初期化
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        throw std::runtime_error(IMG_GetError());
    }
    if((TTF_Init() != 0)){
        throw std::runtime_error(SDL_GetError());
    }
}

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
    /*
     * Gameオブジェクトが破棄されたあとにSDLの終了処理を実施する
     * →リーク回避のため
     * DBusのリークはSDL_Linux側の問題なので無視
     */
    // quitSDL();
}

void Game::bootstrapWindowAndRenderer(){
    // ウィンドウ
    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    // レンダラー
    renderer = std::make_unique<Renderer>(window->get());
}
void Game::loadResources(){
    // プレイヤー
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    // 敵(とりあえず5体表示)
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    // テキスト
    font = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    // スコア
    scoreText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    scoreText->setText("Score: 0");
    // FPS
    fpsText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    fpsText->setText("");
}
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
            enemies,
            blocks
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
void Game::buildScenes(){
    scenes[(int)GameScene::Title]    = std::make_unique<TitleScene>(*this, *ctx);
    scenes[(int)GameScene::Playing]  = std::make_unique<PlayingScene>(*this, *ctx);
    scenes[(int)GameScene::GameOver] = std::make_unique<GameOverScene>(*this, *ctx);
    scenes[(int)GameScene::Clear]    = std::make_unique<ClearScene>(*this, *ctx);
}
void Game::startFromTitle(){
    // タイトルからスタート
    currentScene = scenes[(int)GameScene::Title].get();
    currentScene->onEnter();
}

/**
 * @brief シーンの切り替えを実施する関数
 * 抽象クラスSceneに対して，ポリモーフィズムによる派生クラス生成を実施する
 * 
 * @param id 
 */
void Game::changeScene(GameScene id){
    currentScene->onExit();
    currentScene = scenes[(int)id].get();
    currentScene->onEnter();
}

void Game::requestScene(GameScene id){
    pendingSceneChange = id;
}

void Game::applySceneChangeIfAny(){
    if(!pendingSceneChange){
        return;
    }
    if(currentScene){
        currentScene->onExit();
    }
    currentScene = scenes[nextSceneIndex(*pendingSceneChange)].get();
    currentScene->onEnter();
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
    using clock = std::chrono::high_resolution_clock;
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

/*
void Game::run(){
    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    
    // FPS 計測用(これはミリ秒で十分)
    Uint32 fpsTimer = SDL_GetTicks();
    int fpsCounter = 0;

    while(running){
        // 時間の計測
        // フレーム開始点
        Uint32 frameStartMs = SDL_GetTicks();
        // 時間開始点
        Uint64 nowCounter = SDL_GetPerformanceCounter();
        // ゲーム差分delta
        double delta = static_cast<double>(nowCounter - lastCounter) / static_cast<double>(frequency);
        // 前回の計測点更新
        lastCounter = nowCounter;
        // クランプ：deltaが大きすぎて更新しすぎることを防ぐ
        if(delta > 0.1){
            delta = 0.1;    // 10FPSくらい
        }
        // 各種処理の実施
        processEvents();
        update(delta);
        render();
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
*/

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

/*
void Game::capFrameRate(Uint32 frameStartMs){
    Uint32 frameDuration = SDL_GetTicks() - frameStartMs;
    if(frameDuration < Game::FRAME_DELAY){
        SDL_Delay(Game::FRAME_DELAY - frameDuration);
    }
}
*/

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

/*
Game::Game(){
    // 初期化
    // initSDL();
    sdl = std::make_unique<SdlSystem>();
    // ウィンドウ
    window = std::make_unique<Window>("Test", GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    // レンダラー
    renderer = std::make_unique<Renderer>(window->get());
    
    // プレイヤー
    playerTexture = std::make_unique<Texture>(renderer->get(), "assets/image/rhb.png");
    player = std::make_unique<Player>(*playerTexture);
    player->setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 敵(とりあえず5体表示)
    enemyTexture = std::make_unique<Texture>(renderer->get(), "assets/image/dark_rhb.png");
    enemies.clear();
    // ブロックの描画情報取得
    blocks.clear(); // ロードはloadStage()で実施
    // 世界の広さ
    worldInfo = {static_cast<double>(GameConfig::WINDOW_WIDTH), 
                 static_cast<double>(GameConfig::WINDOW_HEIGHT)
                };
    // ブロックが置かれている範囲に応じて拡張
    for(const auto& b : blocks){
        worldInfo.WorldWidth = std::max(worldInfo.WorldWidth, b.x + b.w);
        worldInfo.WorldHeight = std::max(worldInfo.WorldHeight, b.y + b.h);
    }
    // テキスト
    font = std::make_unique<Text>("assets/font/NotoSansJP-Regular.ttf", 24);
    // スコア
    scoreText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    scoreText->setText("Score: 0");
    // FPS
    fpsText = std::make_unique<TextTexture>(*renderer, *font, SDL_Color{255, 255, 255, 255});
    fpsText->setText("");
    // input抽象化
    input = std::make_unique<Input>();
    // カメラ
    camera = {0.0, 0.0, 
              static_cast<double>(GameConfig::WINDOW_WIDTH), 
              static_cast<double>(GameConfig::WINDOW_HEIGHT)
            };
    
    // running = true;

    // 乱数
    distX = std::uniform_real_distribution<double>(GameConfig::WINDOW_WIDTH/2, GameConfig::WINDOW_WIDTH - EnemyConfig::FRAME_W);
    distY = std::uniform_real_distribution<double>(EnemyConfig::FRAME_H, GameConfig::WINDOW_HEIGHT - EnemyConfig::FRAME_H);
    distSpeed = std::uniform_real_distribution<double>(EnemyConfig::SPEED_MIN, EnemyConfig::SPEED_MAX);

    // シーン
    ctx = std::make_unique<GameContext>(GameContext{
        *renderer,
        *input, 
        camera,
        worldInfo, 
        EntityContext{
            *player,
            *enemyTexture,
            enemies,
            blocks
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
    scenes[(int)GameScene::Title]    = std::make_unique<TitleScene>(*this, *ctx);
    scenes[(int)GameScene::Playing]  = std::make_unique<PlayingScene>(*this, *ctx);
    scenes[(int)GameScene::GameOver] = std::make_unique<GameOverScene>(*this, *ctx);
    scenes[(int)GameScene::Clear]    = std::make_unique<ClearScene>(*this, *ctx);

    // タイトルからスタート
    currentScene = scenes[(int)GameScene::Title].get();
    currentScene->onEnter();
}
*/
