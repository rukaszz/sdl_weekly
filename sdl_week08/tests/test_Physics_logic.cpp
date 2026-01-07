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
 * すり抜け床のパターン
 */
TEST(PhysicsTests, DropThroughTheSlipBlock) {
    // Physicsへ渡すDTO
    VerticalCollisionState vcs{
        .prevFeet          = 100.0, 
        .newFeet           = 100.0, // 床(100.0)を超えている
        .x                 = 50.0, 
        .width             = 20.0, 
        .vv                = 50.0, 
        .onGround          = false, 
        .ignoreDropThrough = true
    };
    // 立てる床ブロック
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
    EXPECT_LE(vcs.newFeet, floor.y);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
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
