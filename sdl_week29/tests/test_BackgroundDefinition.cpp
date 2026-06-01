#include <gtest/gtest.h>

#include "BackgroundDefinition.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * 森の背景で背景構成処理が正常な順番(遠景→近景)で処理されるかテスト
 */
TEST(BackgroundDefinitionTests, ForestWithThreeParallaxObjectBackToFront){
    // Forestのレイヤー構成を取得
    auto layers = BackgroundDefinition::getLayerDefs(BackgroundId::Forest);

    // 3層のレイヤーであるか
    ASSERT_EQ(layers.size(), 3u);   // unsigned int
    // 背景定義を正しく読み込めているかチェック
    // 空
    EXPECT_EQ(layers[0].texId, BgLayerTextureId::Sky);
    EXPECT_DOUBLE_EQ(layers[0].parallaxFactor, 0.0);
    // 山
    EXPECT_EQ(layers[1].texId, BgLayerTextureId::Mountain);
    EXPECT_DOUBLE_EQ(layers[1].parallaxFactor, 0.2);
    // 森
    EXPECT_EQ(layers[2].texId, BgLayerTextureId::Forest);
    EXPECT_DOUBLE_EQ(layers[2].parallaxFactor, 0.4);
}

/**
 * @brief Construct a new TEST object
 * 
 * DarkForest版
 */
TEST(BackgroundDefinitionTests, DarkForestWithThreeParallaxObjectBackToFront){
    // Forestのレイヤー構成を取得
    auto layers = BackgroundDefinition::getLayerDefs(BackgroundId::DarkForest);

    // 3層のレイヤーであるか
    ASSERT_EQ(layers.size(), 3u);   // unsigned int
    // 背景定義を正しく読み込めているかチェック
    // 空
    EXPECT_EQ(layers[0].texId, BgLayerTextureId::DarkSky);
    EXPECT_DOUBLE_EQ(layers[0].parallaxFactor, 0.0);
    // 山
    EXPECT_EQ(layers[1].texId, BgLayerTextureId::DarkMountain);
    EXPECT_DOUBLE_EQ(layers[1].parallaxFactor, 0.2);
    // 森
    EXPECT_EQ(layers[2].texId, BgLayerTextureId::DarkForest);
    EXPECT_DOUBLE_EQ(layers[2].parallaxFactor, 0.4);
}

/**
 * @brief Construct a new TEST object
 * 
 * 森の背景で背景構成処理が正常な順番(遠景→近景)で処理されるかテスト
 */
TEST(BackgroundDefinitionTests, HellForestWithThreeParallaxObjectBackToFront){
    // Forestのレイヤー構成を取得
    auto layers = BackgroundDefinition::getLayerDefs(BackgroundId::HellForest);

    // 3層のレイヤーであるか
    ASSERT_EQ(layers.size(), 3u);   // unsigned int
    // 背景定義を正しく読み込めているかチェック
    // 空
    EXPECT_EQ(layers[0].texId, BgLayerTextureId::HellSky);
    EXPECT_DOUBLE_EQ(layers[0].parallaxFactor, 0.0);
    // 山
    EXPECT_EQ(layers[1].texId, BgLayerTextureId::HellMountain);
    EXPECT_DOUBLE_EQ(layers[1].parallaxFactor, 0.2);
    // 森
    EXPECT_EQ(layers[2].texId, BgLayerTextureId::HellForest);
    EXPECT_DOUBLE_EQ(layers[2].parallaxFactor, 0.4);
}

/**
 * @brief Construct a new TEST object
 * 
 * 各シーン(PlayingScene以外)での1枚画像の背景のテスト
 */
TEST(BackgroundDefinitionTest, SimpleBackgroundWithSingleLayer){
    // 定義読み込み
    auto lightBg = BackgroundDefinition::getLayerDefs(BackgroundId::LightBg);
    // lightBgの検証
    ASSERT_EQ(lightBg.size(), 1u);  // unsigned int
    EXPECT_EQ(lightBg[0].texId, BgLayerTextureId::Sky);
    EXPECT_DOUBLE_EQ(lightBg[0].parallaxFactor, 0.0);

    // 同様にDarkBgの定義読み込み
    auto DarkBg = BackgroundDefinition::getLayerDefs(BackgroundId::DarkBg);
    // lightBgの検証
    ASSERT_EQ(DarkBg.size(), 1u);  // unsigned int
    EXPECT_EQ(DarkBg[0].texId, BgLayerTextureId::DarkSky);
    EXPECT_DOUBLE_EQ(DarkBg[0].parallaxFactor, 0.0);
}

/**
 * @brief Construct a new TEST object
 * 
 * 不明なBackgroundIdを指定した場合に，エラーがthrowされるか
 */
TEST(BackgroundDefinitionTest, UnknownBackgroundIdThrowCheck){
    // 不正なIdを指定(uint8_t範囲内かつ未定義の値: 有効値は0〜4)
    // 256を指定すると，uint_8の0~255の範囲を超えて，0にトランケートされてしまう=Forestになってしまう
    const auto invalidId = static_cast<BackgroundId>(99);
    // エラーが戻ってくることを確認
    EXPECT_THROW((void)BackgroundDefinition::getLayerDefs(invalidId), std::runtime_error);
}
