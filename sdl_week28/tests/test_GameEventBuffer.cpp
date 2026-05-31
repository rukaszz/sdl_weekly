#include <gtest/gtest.h>

#include <variant>
#include <vector>

#include "GameEventBuffer.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * テスト時点でGameEventにあるイベントをバッファへ格納してholds_alternativeでチェックする
 */
TEST(GameEventBufferTests, EnqueueAllEventTypes){
    GameEventBuffer events;
    // イベントの格納
    events.addScore(120);
    events.playSound(SoundId::Fireball);
    events.requestScene(GameScene::Clear);
    events.requestPlayerDeath(100.0, 600.0);
    events.requestGameQuit();
    events.startCameraShake(0.25, 14.0);
    events.spawnParticle(ParticleEffectId::EnemyBurst, 110.0, 550.0);

    // イベントが正しく格納できているか
    // ASSERTを使うので，ここでNGならテストが終わる
    // items()はvectorを返すので，size()を呼び出す
    ASSERT_EQ(events.items().size(), 7u);
    // holds_alternative<T>は型Tのデータがvariantに入っているか調べる(bool)
    EXPECT_TRUE(std::holds_alternative<AddScoreEvent>(events.items()[0]));
    EXPECT_TRUE(std::holds_alternative<PlaySoundEvent>(events.items()[1]));
    EXPECT_TRUE(std::holds_alternative<RequestSceneEvent>(events.items()[2]));
    EXPECT_TRUE(std::holds_alternative<RequestPlayerDeathEvent>(events.items()[3]));
    EXPECT_TRUE(std::holds_alternative<RequestGameQuitEvent>(events.items()[4]));
    EXPECT_TRUE(std::holds_alternative<StartCameraShakeEvent>(events.items()[5]));
    EXPECT_TRUE(std::holds_alternative<SpawnParticleEvent>(events.items()[6]));

    // 座標を指定しているものは別途get<T>()で取得して確認
    // particle
    const auto& particle = std::get<SpawnParticleEvent>(events.items()[6]);
    // 格納時に指定した値であるかみる
    EXPECT_EQ(particle.id, ParticleEffectId::EnemyBurst);
    EXPECT_EQ(particle.x, 110.0);
    EXPECT_EQ(particle.y, 550.0);

    // deathの座標
    const auto& death = std::get<RequestPlayerDeathEvent>(events.items()[3]);
    EXPECT_EQ(death.x, 100.0);
    EXPECT_EQ(death.y, 600.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * イベントシステムの要であるconsumeIfの処理で，
 * 1. 正しく指定したイベントのみが取得できるか
 * 2. 取得したイベントの空きを正しく詰められるか
 * をテスト
 */
TEST(GameEventBufferTests, ConsumeIfRemovesOnlyMatchingEventsAndPreservesOthersOrder){
    GameEventBuffer events;
    // 適当なイベントをいくつかバッファへEnqueue
    // ※addScoreがテスト対象
    events.addScore(10);
    events.playSound(SoundId::Coin);
    events.addScore(20);
    events.spawnParticle(ParticleEffectId::CoinSpark, 1.0, 2.0);
    events.addScore(-5);

    // addScoreの合計/イベント取り出し回数の計測用変数
    int consumedScore = 0;
    int consumedEventCount = 0;

    // イベント取り出し(consumeIf構文)
    events.consumeIf(
        // Pred:　AddScoreEventのみtrueになる
        [](const GameEvent& ev){
            return std::holds_alternative<AddScoreEvent>(ev);
        }, 
        // &でAddScoreEventでtrueになるevをキャプチャ
        [&](const GameEvent& ev){
            // getで取り除かれる
            consumedScore += std::get<AddScoreEvent>(ev).delta;
            ++consumedEventCount;
        }
        // consumeIf内でeventsの取り出しや空白の詰めが行われる
    );
    
    // addScoreイベントが3回発生しており，10+20+(-5)で25点加算されるはず
    EXPECT_EQ(consumedEventCount, 3);
    EXPECT_EQ(consumedScore, 25);

    // eventsの状態確認
    // サイズチェック※ここでNGなら以降はテストしない
    ASSERT_EQ(events.items().size(), 2u);   // addScore*3なので，2つしかイベントが残っていないはず
    // 残っているイベントの確認
    // 順番的にはplaySoundのイベントが0番目にきているはず
    ASSERT_TRUE(std::holds_alternative<PlaySoundEvent>(events.items()[0]));
    EXPECT_EQ(std::get<PlaySoundEvent>(events.items()[0]).id, SoundId::Coin);
    
    // その次に連続してspawnParticleイベントがある
    ASSERT_TRUE(std::holds_alternative<SpawnParticleEvent>(events.items()[1]));
    // 中身を取り出して座標値もチェックしておく
    const auto& particle = std::get<SpawnParticleEvent>(events.items()[1]);
    EXPECT_EQ(particle.id, ParticleEffectId::CoinSpark);
    EXPECT_EQ(particle.x, 1.0);
    EXPECT_EQ(particle.y, 2.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * eventsバッファのclear()処理の確認
 */
TEST(GameEventBufferTests, ClearProcessRemoveAllEvents){
    GameEventBuffer events;
    events.requestScene(GameScene::Title);
    events.requestGameQuit();

    // この時点ではバッファに要素がある
    ASSERT_FALSE(events.items().empty());
    // バッファクリア
    events.clear();
    EXPECT_TRUE(events.items().empty());
}

/**
 * @brief Construct a new TEST object
 * 
 * 空バッファへのconsumeIfの処理
 */
TEST(GameEventBufferTests, ConsumeIfDoesNothingForEmptyQueue){
    GameEventBuffer events;
    
    // addScoreの合計/イベント取り出し回数の計測用変数
    int consumedScore = 0;
    int consumedEventCount = 0;

    // イベント取り出し(consumeIf構文)
    // イベントが無いが適当なイベントの取り出し操作を実施
    events.consumeIf(
        // Pred:　AddScoreEventのみtrueになる
        [](const GameEvent& ev){
            return std::holds_alternative<AddScoreEvent>(ev);
        }, 
        // &でAddScoreEventでtrueになるevをキャプチャ
        [&](const GameEvent& ev){
            // getで取り除かれる
            consumedScore += std::get<AddScoreEvent>(ev).delta;
            ++consumedEventCount;
        }
        // consumeIf内でeventsの取り出しや空白の詰めが行われる
    );
    
    // addScoreイベントが3回発生しており，10+20+(-5)で25点加算されるはず
    EXPECT_EQ(consumedEventCount, 0);
    EXPECT_EQ(consumedScore, 0);

    // eventsの状態確認
    // サイズチェック
    EXPECT_EQ(events.items().size(), 0u);
}
