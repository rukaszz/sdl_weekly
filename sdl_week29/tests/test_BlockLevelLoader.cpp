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
    std::string rsl = R"(S N 0   750 200 50 # 床
T N 200  80 100 20 # 空中の足場(すり抜け)
D N 400 380 100 20 # 空中のダメージ床
C N 650 80 20 20   # クリアオブジェクト
B N 750 80 20 20   # 破壊可能ブロック
U N 850 80 20 20   # 使用済みブロック
Q C 950 80 20 20   # ？ブロック(コイン入り)
Q M 950 100 20 20   # ？ブロック(きのこ入り)
Q F 950 120 20 20   # ？ブロック(ファイアフラワー入り)
E N 1050 80 20 20   # 空のブロック)";

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
            Field(&Block::type, BlockType::Standable), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // DropThrough
        AllOf(
            Field(&Block::x, 200), 
            Field(&Block::y, 80), 
            Field(&Block::w, 100), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::DropThrough), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // Damage
        AllOf(
            Field(&Block::x, 400), 
            Field(&Block::y, 380), 
            Field(&Block::w, 100), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Damage), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // Clear
        AllOf(
            Field(&Block::x, 650), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Clear), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // Breakble
        AllOf(
            Field(&Block::x, 750), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Breakable), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // UsedQuestion
        AllOf(
            Field(&Block::x, 850), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::UsedQuestion), 
            Field(&Block::reward, BlockRewardType::None)
        ), 
        // Question
        AllOf(
            Field(&Block::x, 950), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Question), 
            Field(&Block::reward, BlockRewardType::Coin)
        ), 
        AllOf(
            Field(&Block::x, 950), 
            Field(&Block::y, 100), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Question), 
            Field(&Block::reward, BlockRewardType::Mushroom)
        ), 
        AllOf(
            Field(&Block::x, 950), 
            Field(&Block::y, 120), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Question), 
            Field(&Block::reward, BlockRewardType::FireFlower)
        ), 
        // Empty
        AllOf(
            Field(&Block::x, 1050), 
            Field(&Block::y, 80), 
            Field(&Block::w, 20), 
            Field(&Block::h, 20), 
            Field(&Block::type, BlockType::Empty), 
            Field(&Block::reward, BlockRewardType::None)
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
    std::string rsl = R"(# T N 200  80 100 20 # コメント行
S N 0   750 200 50 # 床(正常なデータ)
D 400 380 100 # カラム欠け
C N 650 80 20 ten   # 変な文字が入る
X N 500 100 10 10 # 未知のBlockTypeは無視)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    // std::vector<Block> blocks;  // 空
    
    // std::runtime_errorを投げるので，それを期待値とする
    EXPECT_THROW(BlockLevelLoader::loadFromFile(filename), std::runtime_error);
    
    // 一時ファイル削除
    std::remove(filename.c_str());
}

/**
 * @brief Construct a new TEST object
 * 
 * 空のレベルテキストデータ読み込み
 */
TEST(BlockLevelLoaderTests, EmptyBlockLevelDataLoad){
    // 一時テキストファイル
    const std::string filename = "temp_test_empty_file.txt";
    // 正常なlevelデータ
    std::string rsl = "";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    // データ読み込み(空vectorを宣言せずにautoで検証)
    auto blocks = BlockLevelLoader::loadFromFile(filename);
    
    EXPECT_TRUE(blocks.empty());

    // 一時ファイル削除
    std::remove(filename.c_str());
}

/**
 * @brief Construct a new TEST object
 * 
 * コメントのみのレベルテキストデータ読み込み(正常のエッジケース)
 */
TEST(BlockLevelLoaderTests, CommentsOnlyBlockLevelDataLoad){
    // 一時テキストファイル
    const std::string filename = "temp_test_empty_file.txt";
    // 正常なlevelデータ
    std::string rsl = R"(
# T N 200  80 100 20 # コメント行
# S N 0   750 200 50 # 床(正常なデータ)
)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);

    // データ読み込み(空vectorを宣言せずにautoで検証)
    auto blocks = BlockLevelLoader::loadFromFile(filename);
    
    EXPECT_TRUE(blocks.empty());

    // 一時ファイル削除
    std::remove(filename.c_str());
}

/**
 * @brief Construct a new TEST object
 * 
 * 存在していないファイルの読み込みテスト
 */
TEST(BlockLevelLoaderTests, BlockLevelDataDoesNotExists){
    
    // 一時テキストファイル
    const std::string filename = "temp_test_does-not_exist_file.txt";
    /*
    // 正常なlevelデータ
    std::string rsl = R"(
# T N 200  80 100 20 # コメント行
# S N 0   750 200 50 # 床(正常なデータ)
)";

    // 一時ファイルへ書き込み
    writeTempFile(filename, rsl);
    */
    // データ読み込み(空vectorを宣言せずにautoで検証)    
    EXPECT_THROW(BlockLevelLoader::loadFromFile(filename), std::runtime_error);

    // 一時ファイル削除
    std::remove(filename.c_str());
}
