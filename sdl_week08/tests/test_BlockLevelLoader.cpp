#include <gtest/gtest.h>

#include <cstdio>   // std::remove用

#include "GameUtil.hpp"
#include "BlockLevelLoader.hpp"
#include "Block.hpp"

/**
 * @brief 一時テキストファイル作成用ヘルパー関数
 * 
 * @param filename 
 * @param content // raw string literalで渡す(R"()")
 */
void writeTempFile(const std::string& filename, const std::string& content){
    // ファイルを開く
    std::ofstream sfs(filename);
    // 開けなければテスト終了
    ASSERT_TRUE(ofs.is_open());
    // ofsへ書き込み
    ofs << content;
}

/**
 * @brief Construct a new TEST object
 * 
 * 正常なデータ読み込み
 */
TEST(AllTest, NormalBlockLevelDataLoad){
    // 一時テキストファイル
    const std::string filename = "temp_test_text_file.txt";
    // 正常なlevelデータ
    std::string rsl = R"(S 0   750 200 50 # 床
T 200  80 100 20 # 空中の足場(すり抜け)
D 400 380 100 20 # 空中のダメージ床
C 650 80 20 20   # クリアオブジェクト)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    std::vector<Block> blocks;  // 空

    block = BlockLevelLoader::loadFromFile(filename);

    // ブロックが空→newFeet/vv/onGroundはそのまま
    EXPECT_DOUBLE_EQ(vcs.newFeet, 100.0);
    EXPECT_DOUBLE_EQ(vcs.vv, 50.0);
    EXPECT_FALSE(vcs.onGround);
}