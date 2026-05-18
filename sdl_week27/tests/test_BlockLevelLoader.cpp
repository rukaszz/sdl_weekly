#include <gtest/gtest.h>
#include <gmock/gmock.h>
using ::testing::ElementsAre;
using ::testing::Field;

#include <fstream>
#include <cstdio>   // std::remove用

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
    std::ofstream ofs(filename);
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
TEST(BlockLevelLoaderTests, NormalBlockLevelDataLoad){
    // 一時テキストファイル
    const std::string filename = "temp_test_normal_file.txt";
    // 正常なlevelデータ
    std::string rsl = R"(S 0   750 200 50 # 床
T 200  80 100 20 # 空中の足場(すり抜け)
D 400 380 100 20 # 空中のダメージ床
C 650 80 20 20   # クリアオブジェクト)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    std::vector<Block> blocks;  // 空
    // データ読み込み
    blocks = BlockLevelLoader::loadFromFile(filename);
    // 期待値vector
    // std::vector<Block> expect = {
    //     {0, 750, 200, 50, Standable}, 
    //     {200, 80, 100, 20, DropThrough}, 
    //     {400, 380, 100, 20, Damage}, 
    //     {650, 80, 20, 20, Clear}
    // }

    EXPECT_THAT(blocks, ElementsAre(
        // Standable
        AllOf(
            Field(&Block::x, 0), 
            Field(&Block::y, 750), 
            Field(&Block::w, 200), 
            Field(&Block::h, 50), 
            Field(&Block::type, BlockType::Standable)
        ), 
        // DropThrough
        AllOf(
            Field(&Block::x, 200), 
            Field(&Block::y, 80), 
            Field(&Block::w, 100), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::DropThrough)
        ), 
        // Damage
        AllOf(
            Field(&Block::x, 400), 
            Field(&Block::y, 380), 
            Field(&Block::w, 100), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Damage)
        ), 
        // Clear
        AllOf(
            Field(&Block::x, 650), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Clear)
        )
    ));
    // 一時ファイル削除
    std::remove(filename.c_str());
}

/**
 * @brief Construct a new TEST object
 * 
 * 壊れたデータを含むテキストデータ読み込み
 */
TEST(BlockLevelLoaderTests, AbnormalBlockLevelDataLoad){
    // 一時テキストファイル
    const std::string filename = "temp_test_abnormal_file.txt";
    // 正常なlevelデータ
    std::string rsl = R"(# T 200  80 100 20 # コメント行
S 0   750 200 50 # 床(正常なデータ)
D 400 380 100 # カラム欠け
C 650 80 20 ten   # 変な文字が入る
X 500 100 10 10 # 未知のBlockTypeは虫)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    std::vector<Block> blocks;  // 空
    // データ読み込み
    blocks = BlockLevelLoader::loadFromFile(filename);
    // 期待値vector
    // 変なデータはスキップされる
    // std::vector<Block> expect = {
    //     {0, 750, 200, 50, Standable}
    // }

    EXPECT_THAT(blocks, ElementsAre(
        // Standableのみ
        AllOf(
            Field(&Block::x, 0), 
            Field(&Block::y, 750), 
            Field(&Block::w, 200), 
            Field(&Block::h, 50), 
            Field(&Block::type, BlockType::Standable)
        )
    ));
    // 一時ファイル削除
    std::remove(filename.c_str());
}
