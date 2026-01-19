#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "PlayerEnemyCollisionUtil.hpp"
#include "PlayerConfig.hpp"

#include <SDL2/SDL.h>

using PlayerEnemyCollision::resolvePlayerEnemyCollision;

/**
 * @brief Construct a new TEST object
 * 
 * 上から踏みつけるパターン
 */
TEST(PlayerEnemyCollisionTests, StompEnemy_FromAbove){
    const SDL_Rect playerRect{40, 25, 20, 20};  // x:40-60, y:25-45 → intersects == true
    const SDL_Rect enemyRect{40, 40, 20, 20};   // x:40-60, y:40-60

    const double enemyTop = enemyRect.y;    // 40

    double playerPrevFeet = enemyTop - 5.0; // 35
    double playerNewFeet  = enemyTop + 5.0; // 45 → Enemyへめり込む
    double playerVv       = 10.0;           // 落下中

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 上からの踏みつけなので，StompEnemyが帰ってくる
    EXPECT_EQ(result, PlayerEnemyCollisionResult::StompEnemy);
}

/**
 * @brief Construct a new TEST object
 * 
 * 側面から衝突するパターン
 */
TEST(PlayerEnemyCollisionTests, PlayerHit_FromSide){
    const SDL_Rect playerRect{55, 45, 20, 20};  // x:55-75, y:45-65
    const SDL_Rect enemyRect{40, 40, 20, 20};   // x:40-60, y:40-60 → 側面から衝突

    double playerPrevFeet = 70.0;
    double playerNewFeet  = 80.0;
    double playerVv       = 10.0;   // 落下中に横からぶつかった

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 側面からの衝突 → PlayerHit
    EXPECT_EQ(result, PlayerEnemyCollisionResult::PlayerHit);
}

/**
 * @brief Construct a new TEST object
 * 
 * 敵と衝突していないとき
 */
TEST(PlayerEnemyCollisionTests, None_WhenNotIntersecting){
    const SDL_Rect playerRect{10, 10, 20, 20};  // x:10-30, y:10-30
    const SDL_Rect enemyRect{200, 200, 20, 20}; // x:200-220, y:200-220

    double playerPrevFeet = 30.0;
    double playerNewFeet  = 40.0;
    double playerVv       = 10.0;

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 何も処理しない
    EXPECT_EQ(result, PlayerEnemyCollisionResult::None);
}

/**
 * @brief Construct a new TEST object
 * 
 * 上からギリギリ踏めていない(境界チェック)
 */
TEST(PlayerEnemyCollisionTests, StompEnemy_AtEdgeOfFailure){
    const SDL_Rect playerRect{40, 25, 20, 20};  // x:40-60, y:25-45 → intersects == true
    const SDL_Rect enemyRect{40, 40, 20, 20};   // x:40-60, y:40-60

    const double enemyTop = enemyRect.y;    // 40

    double playerPrevFeet = enemyTop + 2*PlayerConfig::eps;   // 42※めり込んでいる
    double playerNewFeet = playerPrevFeet + 2*PlayerConfig::eps;   // 44※めり込んでいる
    double playerVv       = 10.0;

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // feetCrossTopの条件未達=敵にヒット
    EXPECT_EQ(result, PlayerEnemyCollisionResult::PlayerHit);
}

/**
 * @brief Construct a new TEST object
 * 
 * 上からギリギリ踏めている(境界チェック)
 */
TEST(PlayerEnemyCollisionTests, StompEnemy_AtEdgeOfSuccess){
    const SDL_Rect playerRect{40, 25, 20, 20};  // x:40-60, y:25-45 → intersects == true
    const SDL_Rect enemyRect{40, 40, 20, 20};   // x:40-60, y:40-60

    const double enemyTop = enemyRect.y;    // 40

    double playerPrevFeet = enemyTop - PlayerConfig::eps;   // 39※ギリギリ踏めていない
    double playerNewFeet = enemyTop + PlayerConfig::eps;   // 41※ギリギリ踏めた
    double playerVv       = 10.0;

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 敵を踏めたはず
    EXPECT_EQ(result, PlayerEnemyCollisionResult::StompEnemy);
}

/**
 * @brief Construct a new TEST object
 * 
 * 落下していないとき
 */
TEST(PlayerEnemyCollisionTests, None_WhenNotFalling){
    const SDL_Rect playerRect{40, 55, 20, 20};  // x:40-60, y:55-75 → intersects == true
    const SDL_Rect enemyRect{40, 55, 20, 20};   // x:40-60, y:55-75

    double playerPrevFeet = 75;
    double playerNewFeet  = 35;      // 35 → Enemyへめり込んではいる
    double playerVv       = -40.0;   // 上昇中(Jumpなど)

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 敵に下からぶつかる→PlayerHit
    EXPECT_EQ(result, PlayerEnemyCollisionResult::PlayerHit);
}


/**
 * @brief Construct a new TEST object
 * 
 * 踏みつけの条件(feetCrossTop)は満たしているが，衝突はしていない場合
 */
TEST(PlayerEnemyCollisionTests, None_StompConditionWithoutIntersection){
    const SDL_Rect playerRect{10, 190, 20, 20};  // x:10-190, y:10-30
    const SDL_Rect enemyRect{200, 200, 20, 20}; // x:200-220, y:200-220

    const double enemyTop = enemyRect.y;    // 40

    double playerPrevFeet = enemyRect.y;
    double playerNewFeet  = enemyRect.y + 10.0;
    double playerVv       = 10.0;

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 何も処理しない
    EXPECT_EQ(result, PlayerEnemyCollisionResult::None);
}

/**
 * @brief Construct a new TEST object
 * 
 * 落下していない状態で接触したパターン
 */
TEST(PlayerEnemyCollisionTests, PlayerHit_OnTheGround){
    const SDL_Rect playerRect{45, 45, 20, 20};  // x:45-75, y:45-65
    const SDL_Rect enemyRect{50, 45, 20, 20};   // x:50-70, y:40-60 → 側面から衝突

    double playerPrevFeet = 70.0;
    double playerNewFeet  = 70.0;
    double playerVv       = 0.0;   // 落下中に横からぶつかった

    auto result = resolvePlayerEnemyCollision(
        playerRect,
        playerPrevFeet,
        playerNewFeet,
        playerVv,
        enemyRect
    );
    // 接地時の衝突 → PlayerHit
    EXPECT_EQ(result, PlayerEnemyCollisionResult::PlayerHit);
}
