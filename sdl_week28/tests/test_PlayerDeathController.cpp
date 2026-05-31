#include <gtest/gtest.h>

#include "PlayerDeathController.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * Playerが死亡していない場合は，何もしないことを確認する
 */
TEST(PlayerDeathControllerTest, UpdateDoesNothingInactive){
    PlayerDeathController death;
    // visibleがupdate前後で変化しない
    EXPECT_TRUE(death.isVisible());
    EXPECT_FALSE(death.update(10.0));   // active=falseなので即return 
    EXPECT_TRUE(death.isVisible());
}

/**
 * @brief Construct a new TEST object
 * 
 * 点滅処理用のvisibleのトグル処理のチェック
 */
TEST(PlayerDeathControllerTest, BlinkTogglesAfterInterval){
    // 演出開始
    PlayerDeathController death;
    death.start();  // PlayerDeathControllerをアクティブにする(blinkVisibleも最初はtrue)

    // BLINK_INTERVAL=0.1なので，その境界で確認する
    EXPECT_TRUE(death.isVisible());     // 最初はtrue
    EXPECT_FALSE(death.update(0.099));  // 演出時間は1.0sなので，まだfalseが返るはず
    EXPECT_TRUE(death.isVisible());     // 0.1s経過していないのでtrue
    // 0.1s経過させる
    EXPECT_FALSE(death.update(0.099));  // 演出時間は1.0sなので，まだfalseが返る
    EXPECT_FALSE(death.isVisible());     // 0.1s経過してtrue→falseになる
}

/**
 * @brief Construct a new TEST object
 * 
 * 死亡演出処理のupdateの戻り値チェック
 */
TEST(PlayerDeathControllerTest, DeathDirectionAfterDuration){
    // 演出開始
    PlayerDeathController death;
    death.start();
    // 0.5ずつ時間経過させてupdateをチェック
    EXPECT_FALSE(death.update(0.5));
    // 1.0s経ってtrueになる
    EXPECT_TRUE(death.update(0.5));
    // 死亡演出が終了したら(1.0s経ったら)非activeになる→trueを返し続けないはず
    EXPECT_FALSE(death.update(0.5));
}

/**
 * @brief Construct a new TEST object
 * 
 * リセット処理のチェック
 */
TEST(PlayerDeathControllerTest, ResetReturnsToInitialState){
    // 演出開始
    PlayerDeathController death;
    death.start();
    death.update(0.1);
    // テスト済みだがFALSEになることを見ておく※前提条件なのでASSERT
    ASSERT_FALSE(death.isVisible());
    // reset()呼び出し
    death.reset();
    // 変数は初期値になっている
    EXPECT_TRUE(death.isVisible());
    EXPECT_FALSE(death.update(1.0));    // 非アクティブなのでfalseが返る
}
