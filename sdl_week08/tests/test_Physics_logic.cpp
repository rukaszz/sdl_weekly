#include <gtest/gtest.h>

#include "Physics.hpp"
#include "Block.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * ブロックがないパターン
 */
TEST(PhysicsTests, FallWithoutBlocks_NoLanding){
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet          = 90.0, 
        .newFeet           = 100.0, 
        .x                 = 50.0, 
        .width             = 20.0, 
        .vv                = 50.0, 
        .onGround          = false, 
        .ignoreDropThrough = false
    };
    std::vector<Block> blocks;  // 空

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // ブロックが空→newFeet/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newFeet, 100.0);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * 立てる床へたったパターン
 */
TEST(PhysicsTests, LandOnStandableBlock){
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet          = 90.0, 
        .newFeet           = 110.0, // 床(100.0)を超えている
        .x                 = 50.0, 
        .width             = 20.0, 
        .vv                = 50.0, 
        .onGround          = false, 
        .ignoreDropThrough = false
    };
    // 立てる床ブロック
    Block floor{
        .x = 40.0,
        .y = 100.0, // blockTop
        .w = 100.0, 
        .h = 20.0, 
        .type = BlockType::Standable
    };
    std::vector<Block> blocks{floor};

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // newFeetはblockTopへクランプされる
    EXPECT_DOUBLE_EQ(vcs.newFeet, floor.y);
    // 着地するので速度とフラグが変わる
    EXPECT_DOUBLE_EQ(vcs.vv, 0.0);
    EXPECT_TRUE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * x軸が離れているパターン
 */
TEST(PhysicsTests, NoCollisionWhenNotOverlappingHorizontally) {
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet = 90.0,
        .newFeet  = 110.0,
        .x        = 200.0, 
        .width    = 20.0,
        .vv       = 50.0,
        .onGround = false, 
        .ignoreDropThrough = false
    };
    // 立てる床ブロック
    Block floor{
        .x = 40.0,   
        .y = 100.0,
        .w = 100.0,
        .h = 20.0,
        .type = BlockType::Standable
    };
    std::vector<Block> blocks{floor};

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // x が離れているので接地しない→newFeet/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newFeet, 110.0);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床で立つパターン
 */
TEST(PhysicsTests, DropThrough_ActsAsStandableWhenNotIgnored) {
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet          = 90.0,  // 上からの落下 
        .newFeet           = 110.0, // 床(100.0)を超えている
        .x                 = 50.0, 
        .width             = 20.0, 
        .vv                = 50.0, 
        .onGround          = false, 
        .ignoreDropThrough = false  // すり抜けない
    };
    // すり抜け床ブロック
    Block floor{
        .x = 40.0,
        .y = 100.0, // blockTop
        .w = 100.0, 
        .h = 20.0, 
        .type = BlockType::DropThrough
    };
    std::vector<Block> blocks{floor};

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // すり抜けるのでonGroundが変わる
    EXPECT_DOUBLE_EQ(vcs.newFeet, floor.y);
    EXPECT_DOUBLE_EQ(vcs.vv, 0.0);  // 接地状態
    EXPECT_TRUE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床をすり抜けるパターン
 */
TEST(PhysicsTests, DropThrough_IgnoredWhenFlagSet_DropStarts) {
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet          = 100.0,   // blockTop と一致 → 直前フレームでは床に立っていた
        .newFeet           = 105.0,   // ほんの少しだけ下に進んだ
        .x                 = 50.0,
        .width             = 20.0,
        .vv                = 30.0,    // 下向き速度 > 0 （落下中）
        .onGround          = true,    // 前フレームは接地中
        .ignoreDropThrough = true     // このフレームだけ床を無視して落ちたい
    };
    // すり抜け床ブロック
    Block floor{
        .x    = 40.0,
        .y    = 100.0,                // blockTop
        .w    = 100.0,
        .h    = 20.0,
        .type = BlockType::DropThrough
    };
    std::vector<Block> blocks{floor};

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // DropThrough を無視しているので「着地しない」＝ newFeet, vv はそのまま / onGround=false
    EXPECT_DOUBLE_EQ(vcs.newFeet, 105.0);   // clamp されない
    EXPECT_DOUBLE_EQ(vcs.vv, 30.0);         // 速度も維持
    EXPECT_FALSE(vcs.onGround);             // 接地から落下状態へ移行→false
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床をすり抜けたあとにその床へ戻らないか
 */
TEST(PhysicsTests, DropThrough_OncePassedTop_DoesNotRelandLater) {
    // すり抜け床ブロック
    Block floor{
        .x    = 40.0,
        .y    = 100.0,            // blockTop
        .w    = 100.0,
        .h    = 20.0,
        .type = BlockType::DropThrough
    };
    std::vector<Block> blocks{floor};

    // フレーム N: 床の上に立っている状態で，このフレームで「落ちる」入力を実施
    VerticalCollisionState dropFrame{
        .prevFeet          = 100.0,  // 床(100.0)の上
        .newFeet           = 105.0,  // 少し下へ進む
        .x                 = 50.0,
        .width             = 20.0,
        .vv                = 30.0,   // 下向きの垂直速度
        .onGround          = true,
        .ignoreDropThrough = true    // このフレームだけ無視の入力
    };
    // フレームNの処理
    Physics::resolveVerticalBlockCollision(dropFrame, blocks);

    // この時点で床を離れている想定
    EXPECT_FALSE(dropFrame.onGround);
    EXPECT_GT(dropFrame.newFeet, floor.y);

    // フレーム N+1: blockTopより下にいる状態で，ignoreDropThroughはfalseに戻っている
    VerticalCollisionState nextFrame{
        .prevFeet          = dropFrame.newFeet,         // 105.0
        .newFeet           = dropFrame.newFeet + 10.0,  // さらに落下した → 115.0
        .x                 = 50.0,
        .width             = 20.0,
        .vv                = 30.0,
        .onGround          = false,
        .ignoreDropThrough = false    // もう無視フラグはfalseに戻っている
    };
    // フレームN+1の処理
    Physics::resolveVerticalBlockCollision(nextFrame, blocks);

    // prevFeet > blockTop なので縦方向条件(prevFeet <= blockTop)を満たさない
    // clampされず2度とこの床には乗らない
    EXPECT_DOUBLE_EQ(nextFrame.newFeet, 115.0);
    EXPECT_DOUBLE_EQ(nextFrame.vv, 30.0);
    EXPECT_FALSE(nextFrame.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * 着地処理に関係しないブロックのパターン
 */
TEST(PhysicsTests, IgnoreDamageAndClearBlocksForLanding) {
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet = 90.0,
        .newFeet  = 110.0,
        .x        = 50.0,
        .width    = 20.0,
        .vv       = 50.0,
        .onGround = false, 
        .ignoreDropThrough = false
    };
    // ダメージ床
    Block damage{
        .x = 40.0,
        .y = 100.0,
        .w = 100.0,
        .h = 20.0,
        .type = BlockType::Damage
    };
    // クリアオブジェクト
    Block clear{
        .x = 40.0,
        .y = 150.0,
        .w = 100.0,
        .h = 20.0,
        .type = BlockType::Clear
    };
    std::vector<Block> blocks{damage, clear};

    Physics::resolveVerticalBlockCollision(vcs, blocks);

    // DamageとClear は着地処理には使われない前提なので無視される→newFeet/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newFeet, 110.0);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
}
