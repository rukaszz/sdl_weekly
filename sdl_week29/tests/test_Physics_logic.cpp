#include <gtest/gtest.h>

#include "Physics.hpp"
#include "Block.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * ブロックがないパターン
 */
TEST(PhysicsTests, FallWithoutBlocksNoLanding){
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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

    // すり抜け床だがignoreDropThrough=falseなので通常ブロックとして着地する
    EXPECT_DOUBLE_EQ(vcs.newFeet, floor.y);
    EXPECT_DOUBLE_EQ(vcs.vv, 0.0);  // 接地状態
    EXPECT_TRUE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床をすり抜けるパターン
 */
TEST(PhysicsTests, DropThrough_IgnoredWhenFlagSetDropStarts) {
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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

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
TEST(PhysicsTests, DropThrough_OncePassedTopDoesNotRelandLater) {
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
    Physics::resolveBlockCollisionFromTop(dropFrame, blocks);

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
    Physics::resolveBlockCollisionFromTop(nextFrame, blocks);

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

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

    // DamageとClear は着地処理には使われない前提なので無視される→newFeet/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newFeet, 110.0);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * ブロックなしのテスト(FromBottom)
 */
TEST(PhysicsTests, JumpWithoutBlocksNoLanding){
    // ブロックなし
    std::vector<Block> blocks;  // 空
    // 縦方向の衝突判定
    VerticalCollisionState vcs{};
    vcs.prevTop = 105.0;
    vcs.newTop = 95.0;      // 10px上
    vcs.prevFeet = 145.0;
    vcs.newFeet = 135.0;    // 10px上
    vcs.x = 50.0;
    vcs.width = 20.0;
    vcs.vv = -100.0;        // 上方向への移動速度
    vcs.onGround = false;   // 上昇しているのでfalse
    vcs.ignoreDropThrough = false;

    Physics::resolveBlockCollisionFromBottom(vcs, blocks);

    // ブロックが空→newTop/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newTop, 95.0); 
    EXPECT_DOUBLE_EQ(vcs.vv, -100.0);
    EXPECT_FALSE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床で立つパターン(FromBottom)
 */
TEST(PhysicsTests, JumpToDropThroughFromBottom) {
    // すり抜け床ブロック
    Block floor{
        .x = 40.0,
        .y = 100.0, // blockTop
        .w = 100.0, 
        .h = 20.0, 
        .type = BlockType::DropThrough
    };
    // Physicsへ渡すDTO
    // 縦方向の衝突判定
    VerticalCollisionState vcs{};
    vcs.prevTop = 100.0;
    vcs.newTop = 70.0;      // 30px上
    vcs.prevFeet = 130.0;
    vcs.newFeet = 100.0;    // 30px上
    vcs.x = 50.0;
    vcs.width = 20.0;
    vcs.vv = -100.0;        // 上方向への移動速度
    vcs.onGround = false;   // 上昇しているのでfalse
    vcs.ignoreDropThrough = false;
    
    std::vector<Block> blocks{floor};

    Physics::resolveBlockCollisionFromBottom(vcs, blocks);

    // すり抜け床へ下から通るとすり抜けるので，vcsは変化しない
    EXPECT_DOUBLE_EQ(vcs.newFeet, 100.0);
    EXPECT_DOUBLE_EQ(vcs.vv, -100.0);
    EXPECT_FALSE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * すり抜け床をすり抜けるパターン(FromBottomのvv>=0のパターンチェック)
 */
TEST(PhysicsTests, DropThrough_IgnoredWhenFlagSetDropStartsFromBottom) {
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
    // vv >= 0なので即return
    Physics::resolveBlockCollisionFromBottom(vcs, blocks);

    // vcsは変化しない
    EXPECT_DOUBLE_EQ(vcs.newFeet, 105.0);   // clamp されない
    EXPECT_DOUBLE_EQ(vcs.vv, 30.0);         // 速度も維持
    EXPECT_TRUE(vcs.onGround);              // trueのまま
}

/**
 * @brief Construct a new TEST object
 * 
 * ?ブロックへの下からのヒット時のインデックスと垂直方向の速度のテスト
 */
TEST(PhysicsTests, HitQuestionBlockFromBottomSetsHitIndexAndStopsVelocity){
    // インデックステスト用に2つブロックを準備
    std::vector<Block> blocks{
        Block{100.0, 100.0, 50.0, 20.0, BlockType::Standable},
        Block{40.0, 80.0, 50.0, 20.0, BlockType::Question},
    };
    // 縦方向の衝突判定
    VerticalCollisionState vcs{};
    vcs.prevTop = 105.0;
    vcs.newTop = 95.0;      // 10px上
    vcs.prevFeet = 145.0;
    vcs.newFeet = 135.0;    // 10px上
    vcs.x = 50.0;
    vcs.width = 20.0;
    vcs.vv = -100.0;        // 上方向への移動速度
    vcs.onGround = false;   // 上昇しているのでfalse
    vcs.ignoreDropThrough = false;

    Physics::resolveBlockCollisionFromBottom(vcs, blocks);

    EXPECT_DOUBLE_EQ(vcs.newTop, 100.0);    // Question の底辺座標 = 80 + 20 と一緒になる→ブロックにめり込まない
    EXPECT_DOUBLE_EQ(vcs.vv, 0.0);          // ぶつかって速度が0になる
    EXPECT_TRUE(vcs.hitCeilingBlock);       // 頭上のブロックにぶつかったのでTrueになる
    EXPECT_EQ(vcs.hitBlockIndex, 1u);       // hitしたブロックは1番目なのでunsigned intの1に等しい
}

/**
 * @brief Construct a new TEST object
 * 
 * ブロックに右からぶつかったときの補正テスト
 */
TEST(PhysicsTests, MovingRightIntoWallPushesEntityLeftAndStopsVelocity){
    // 壁ブロック用意
    std::vector<Block> blocks{
        Block{100.0, 50.0, 20.0, 100.0, BlockType::Standable},
    };
    // 横方向への衝突判定
    HorizontalCollisionState hcs{};
    hcs.x = 90.0;
    hcs.y = 70.0;
    hcs.width = 20.0;   // 90 + 20 = 110 なのでブロックにめり込む
    hcs.height = 30.0;
    hcs.hv = 120.0;     // 右側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcs, blocks);
    
    // エンティティの右座標110, blockLeft が100, 10 のx軸の重なりが解消される
    // →エンティティの右座標が100になるので，x座標(左側)は80
    EXPECT_DOUBLE_EQ(hcs.x, 80.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcs.hv, 0.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * ブロックに左からぶつかったときの補正テスト
 */
TEST(PhysicsTests, MovingLeftIntoWallPushesEntityRightAndStopsVelocity){
    // 右のときと同じブロックを用意
    std::vector<Block> blocks{
        Block{100.0, 50.0, 20.0, 100.0, BlockType::Standable},
    };
    // 横方向への衝突判定用
    HorizontalCollisionState hcs{};
    hcs.x = 110.0;      // 左側が110でめり込んでいる
    hcs.y = 70.0;
    hcs.width = 20.0;
    hcs.height = 30.0; 
    hcs.hv = -120.0;    // 左側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcs, blocks);

    // エンティティに左座標(x座標)が110, blockRight 120, なので10重なっている
    // →補正されてエンティティの左座標(x座標)が120になる(ぴったりくっついてる)
    EXPECT_DOUBLE_EQ(hcs.x, 120.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcs.hv, 0.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * ブロックへ垂平方向にぶつかってすり抜けるか
 */
TEST(PhysicsExtraTests, HorizontalCollisionIgnoresThroughBlocks){
    // 水平方向はすり抜けるブロックを用意
    std::vector<Block> blocks{
        Block{100.0, 50.0, 20.0, 100.0, BlockType::DropThrough},
        Block{100.0, 50.0, 20.0, 100.0, BlockType::Damage},
        Block{100.0, 50.0, 20.0, 100.0, BlockType::Clear},
    };
    // 水平方向移動要素
    HorizontalCollisionState hcs{};
    hcs.x = 90.0;
    hcs.y = 70.0;
    hcs.width = 20.0;   // 90 + 20 = 110 なのでブロックにめり込む
    hcs.height = 30.0;
    hcs.hv = 120.0;     // 右側へ移動中

    Physics::resolveHorizontalBlockCollision(hcs, blocks);

    // 上記のhcs要素は何も補正されないので変化しない
    EXPECT_DOUBLE_EQ(hcs.x, 90.0);
    EXPECT_DOUBLE_EQ(hcs.hv, 120.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * 立てる床が重なっているパターン
 */
TEST(PhysicsTests, LandOnOverlappingFloor){
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

    // 乗る床(重なっていて上にある)
    Block rideable{40.0, 100.0, 100.0, 20.0, BlockType::Standable};
    // 乗らない床(重なっていて下にある)
    Block nonRideable{50.0, 110.0, 100.0, 20.0, BlockType::Standable};
    std::vector<Block> blocks{rideable, nonRideable};

    Physics::resolveBlockCollisionFromTop(vcs, blocks);

    // newFeetはblockTopへクランプされる
    EXPECT_DOUBLE_EQ(vcs.newFeet, rideable.y);
    // 着地するので速度とフラグが変わる
    EXPECT_DOUBLE_EQ(vcs.vv, 0.0);
    EXPECT_TRUE(vcs.onGround);
}

/**
 * @brief Construct a new TEST object
 * 
 * 破壊可能ブロックに左からぶつかったときの補正テスト
 */
TEST(PhysicsTests, MovingLeftAndRightIntoBreakableBlockPushesRightAndStopsVelocity){
    // 壊せるブロックを用意
    std::vector<Block> blocks{
        Block{100.0, 50.0, 20.0, 100.0, BlockType::Breakable},
    };
    // 左方向への衝突判定用
    HorizontalCollisionState hcsLeft{};
    hcsLeft.x = 115.0;      // ブロックの右辺120へエンティティの左側が115でめり込んでいる→120に補正されるはず
    hcsLeft.y = 70.0;
    hcsLeft.width = 20.0;
    hcsLeft.height = 30.0; 
    hcsLeft.hv = -120.0;    // 左側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcsLeft, blocks);

    // 右方向への衝突判定用
    HorizontalCollisionState hcsRight{};
    hcsRight.x = 85.0;
    hcsRight.y = 70.0;
    hcsRight.width = 20.0;  // ブロックの左辺100へ85+20=105で5めりこむ→エンティティのxが80に補正されるはず
    hcsRight.height = 30.0; 
    hcsRight.hv = 120.0;    // 右側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcsRight, blocks);

    // エンティティに左座標(x座標)が110, blockRight 120, なので10重なっている
    // →補正されてエンティティの左座標(x座標)が120になる(ぴったりくっついてる)
    // 左
    EXPECT_DOUBLE_EQ(hcsLeft.x, 120.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcsLeft.hv, 0.0);
    // 右
    EXPECT_DOUBLE_EQ(hcsRight.x, 80.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcsRight.hv, 0.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * 使用済み？ブロックに水平方向にぶつかったときの補正テスト
 */
TEST(PhysicsTests, MovingLeftAndRightIntoUsedQuestionBlockPushesRightAndStopsVelocity){
    // 壊せるブロックを用意
    std::vector<Block> blocks{
        Block{100.0, 50.0, 20.0, 100.0, BlockType::UsedQuestion},
    };
    // 左方向への衝突判定用
    HorizontalCollisionState hcsLeft{};
    hcsLeft.x = 115.0;      // ブロックの右辺120へエンティティの左側が115でめり込んでいる→120に補正されるはず
    hcsLeft.y = 70.0;
    hcsLeft.width = 20.0;
    hcsLeft.height = 30.0; 
    hcsLeft.hv = -120.0;    // 左側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcsLeft, blocks);

    // 右方向への衝突判定用
    HorizontalCollisionState hcsRight{};
    hcsRight.x = 85.0;
    hcsRight.y = 70.0;
    hcsRight.width = 20.0;  // ブロックの左辺100へ85+20=105で5めりこむ→エンティティのxが80に補正されるはず
    hcsRight.height = 30.0; 
    hcsRight.hv = 120.0;    // 右側への水平方向移動速度

    Physics::resolveHorizontalBlockCollision(hcsRight, blocks);

    // エンティティに左座標(x座標)が110, blockRight 120, なので10重なっている
    // →補正されてエンティティの左座標(x座標)が120になる(ぴったりくっついてる)
    // 左
    EXPECT_DOUBLE_EQ(hcsLeft.x, 120.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcsLeft.hv, 0.0);
    // 右
    EXPECT_DOUBLE_EQ(hcsRight.x, 80.0);
    // 水平方向移動速度は0になる
    EXPECT_DOUBLE_EQ(hcsRight.hv, 0.0);
}
