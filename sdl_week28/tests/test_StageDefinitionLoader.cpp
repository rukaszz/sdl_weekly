#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "StageDefinitionLoader.hpp"
#include "StageDefinition.hpp"
#include "BackgroundId.hpp"

namespace{
    /**
     * @brief テスト用のstage.jsonを作成するためのヘルパ関数
     * 
     * @param path: ファイル作成先
     * @param content: ファイルへ出力する文字列 
     */
    void writeFile(const std::string path, const std::string& content){
        std::ofstream ofs(path);
        ASSERT_TRUE(ofs.is_open());
        ofs << content;
    }
}   // namespace

TEST(StageDefinitionLoaderTests, LoadsBackgroundDecorationsBossEnemiesAndItems){
    // jsonファイル作成
    const std::string path = "temp_stage_definition.json";
    writeFile(path, R"(
{
  "stages": [
    {
      "name": "stage-test",
      "levelFile": "level_test.txt",
      "playerStart": {"x": 10, "y": 20},
      "background": "DarkForest",
      "bgDecorations": [
        {"type": "Star", "world_X": 100, "screen_Y": 30, "parallaxFactor": 0.1},
        {"type": "DarkSun", "world_X": 200, "screen_Y": 50, "parallaxFactor": 0.0}
      ],
      "boss": {
        "enabled": true,
        "trigger_X": 1000,
        "cameraMin_X": 900,
        "cameraMax_X": 1600,
        "spawn_X": 1300,
        "spawn_Y": 500,
        "hp": 5
      },
      "enemies": [
        {"type": "Walker", "x": 100, "y": 200, "speed": 60},
        {"type": "Turret", "x": 300, "y": 200, "speed": 0}
      ],
      "items": [
        {"type": "Coin", "x": 400, "y": 300},
        {"type": "FireFlower", "x": 450, "y": 300}
      ]
    }
  ]
}
)");
    // 上記のステージ定義jsonファイルを読み込み
    auto stageDef = StageDefinitionLoader::loadStagesFromJson(path);
    // ステージ定義を読み込んだのでファイルは不要
    std::remove(path.c_str());
    // テスト部分
    // 読み込んだステージ定義をチェック
    ASSERT_EQ(stageDef.size(), 1u); // vectorに1つ格納されているはず
    const auto& s = stageDef[0];    // ステージ定義を取得
    // jsonの各要素が正しく読めているか
    EXPECT_EQ(s.name, "stage-test");
    EXPECT_EQ(s.backgroundId, BackgroundId::DarkForest);
    ASSERT_EQ(s.bgDecorations.size(), 2u);
    EXPECT_EQ(s.bgDecorations[0].type, BgDecorationType::Star);
    EXPECT_EQ(s.bgDecorations[1].type, BgDecorationType::DarkSun);
    EXPECT_TRUE(s.bossBattleDef.enabled);
    EXPECT_DOUBLE_EQ(s.bossBattleDef.trigger_X, 1000.0);
    EXPECT_EQ(s.bossBattleDef.hp, 5);
    ASSERT_EQ(s.enemySpawns.size(), 2u);
    EXPECT_EQ(s.enemySpawns[1].type, EnemyType::Turret);
    ASSERT_EQ(s.itemSpawns.size(), 2u);
    EXPECT_EQ(s.itemSpawns[1].type, ItemType::FireFlower);
}

/**
 * @brief Construct a new TEST object
 * 
 * 背景定義が無いjsonを読み込んだ際の処理チェック
 * ※その他不正なJsonの読み込みテストは省略
 */
TEST(StageDefinitionLoaderTests, LoadsUnknownBackgroundAndThrows){
    const std::string path = "temp_stage_definition_has_bad_bg.json";
    writeFile(path, R"(
{
  "stages": [
    {
      "name": "bad",
      "levelFile": "level.txt",
      "playerStart": {"x": 0, "y": 0},
      "background": "NoSuchBackground"
    }
  ]
}
)");
    // エラーが帰ってくるかチェック
    EXPECT_THROW((void)StageDefinitionLoader::loadStagesFromJson(path), std::runtime_error);
    // ファイルは片付ける
    std::remove(path.c_str());
}

/**
 * @brief Construct a new TEST object
 * 
 * 存在しないjsonを読み込んだ際の処理チェック
 */
TEST(StageDefinitionLoaderTests, LoadsNonexistsJsonFile){
    const std::string path = "temp_stage_definition_does_not_exist.json";
/*
    writeFile(path, R"()");
*/
    // エラーが帰ってくるかチェック
    EXPECT_THROW((void)StageDefinitionLoader::loadStagesFromJson(path), std::runtime_error);
    // ファイルは片付ける
    std::remove(path.c_str());
}
