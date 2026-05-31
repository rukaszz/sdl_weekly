#include <gtest/gtest.h>

#include "GameUtil.hpp"

#include <string>

#include <SDL2/SDL.h>

/* ---------- GameUtil::trim() ---------- */
/**
 * @brief Construct a new TEST object
 * 
 * 先頭と末尾の半角スペースが取り除かれるパターン
 */
TEST(GameUtilTests, GameUtil_TrimSpace){
    // テスト用文字列
    std::string test_str = " S 10 20 30 40 ";
    // 期待値
    std::string expect_str = "S 10 20 30 40";
    GameUtil::trim(test_str);
    // テスト
    EXPECT_EQ(test_str, expect_str);
}

/**
 * @brief Construct a new TEST object
 * 
 * 半角スペースのみの行が空の文字列にトリムされるパターン
 */
TEST(GameUtilTests, GameUtil_TrimBlankLine){
    // テスト用文字列
    std::string test_str = "          "; // 半角スペース10個
    // 期待値
    std::string expect_str = "";
    GameUtil::trim(test_str);
    // テスト
    EXPECT_EQ(test_str, expect_str);
}

/**
 * @brief Construct a new TEST object
 * 
 * すでにTrim()されている文字列への再適用(冪等性)
 */
TEST(GameUtilTests, GameUtil_TrimAlreadyTrimmed){
    std::string s = "hello";
    GameUtil::trim(s);
    EXPECT_EQ(s, "hello");  // 変化しない
}

/**
 * @brief Construct a new TEST object
 * 
 * 空文字へのTrim()
 */
TEST(GameUtilTests, GameUtil_TrimEmpty){
    std::string s = "";
    GameUtil::trim(s);
    EXPECT_EQ(s, "");
}

/**
 * @brief Construct a new TEST object
 * 
 * タブ文字へのTrim()
 * ※std::isspaceはタブも空白と判定する
 */
TEST(GameUtilTests, GameUtil_TrimTabCharacters){
    std::string s = "\t hello \t";
    GameUtil::trim(s);
    EXPECT_EQ(s, "hello");
}

/* ---------- GameUtil::intersects() ---------- */
/**
 * @brief Construct a new TEST object
 * 
 * 明らかに接触しているパターン
 */
TEST(GameUtilTests, GameUtil_AABBClearlyIntersecting){
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{5, 5, 15, 15};

    EXPECT_TRUE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 明らかに接触していないパターン
 */
TEST(GameUtilTests, GameUtil_AABBClearlyNotIntersecting){
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{20, 20, 30, 30};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 左右の境界で接触しているパターン
 */
TEST(GameUtilTests, GameUtil_AABBTouchAtEdgeHorizontal){
    // aの右端(10)とbの左端(10)が接触→false
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{10, 0, 20, 10};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 上下の境界で接触しているパターン
 */
TEST(GameUtilTests, GameUtil_AABBTouchAtEdgeVertical){
    // aの上端(10)とbの下端(10)が接触→false
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{0, 10, 10, 20};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 角で接触しているパターン
 */
TEST(GameUtilTests, GameUtil_AABBTouchAtCorner){
    // 角(a右下とb左上)が接触→false
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{10, 10, 10, 20};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}

/* ---------- GameUtil::blockToRect() ---------- */
/**
 * @brief Construct a new TEST object
 * 
 * blockToRect()の変換処理のチェック
 */
TEST(GameUtilTests, GameUtil_BlockToRect_ConvertsCorrectly){
    // Stabdableのブロックを用意
    Block b{10.0, 20.0, 100.0, 50.0, BlockType::Standable};
    // blockToRect()呼び出し
    SDL_Rect r = GameUtil::blockToRect(b);
    // 結果確認※上記のBlockの初期化に一致するか
    EXPECT_EQ(r.x, 10);
    EXPECT_EQ(r.y, 20);
    EXPECT_EQ(r.w, 100);
    EXPECT_EQ(r.h, 50);
}

/**
 * @brief Construct a new TEST object
 * 
 * blockToRect()の変換時のキャスト処理のチェック
 */
TEST(GameUtilTests, GameUtil_BlockToRect_TruncatesFloatingPoint){
    // Stabdableのブロックを用意(小数点のデータ)
    Block b{1.9, 2.7, 30.5, 40.1, BlockType::Standable};
    // blockToRect()呼び出し
    SDL_Rect r = GameUtil::blockToRect(b);
    // 結果確認※小数点は切り捨てになるはず
    EXPECT_EQ(r.x, 1);
    EXPECT_EQ(r.y, 2);
    EXPECT_EQ(r.w, 30);
    EXPECT_EQ(r.h, 40);
}

/* ---------- GameUtil::rebuildBlockRects() ---------- */
/**
 * @brief Construct a new TEST object
 * 
 * ブロックのキャッシュ構成で，空vectorなら空vectorが生成されるか
 */
TEST(GameUtilTests, GameUtil_rebuildBlockRects_EmptyBlocks){
    // 空vector
    std::vector<Block> blocks;
    std::vector<SDL_Rect> rects;
    // rebuildBlockRects()呼び出し
    GameUtil::rebuildBlockRects(blocks, rects);
    // rectsは空のはず
    EXPECT_TRUE(rects.empty());
}

/**
 * @brief Construct a new TEST object
 * 
 * 呼び出し→キャッシュ構成が正常に実施されるか
 */
TEST(GameUtilTests, GameUtil_RebuildBlockRects_BuildsCorrectly){
    // 座標やBlockTypeの異なるBlockを2つ用意
    std::vector<Block> blocks{
        {10.0, 20.0, 100.0, 50.0, BlockType::Standable}, 
        {200.0, 300.0, 40.0, 20.0, BlockType::DropThrough}, 
    };
    // キャッシュの受け皿用意(rests)
    std::vector<SDL_Rect> rects;
    // キャッシュ作成
    GameUtil::rebuildBlockRects(blocks, rects);
    // 結果チェック
    // サイズが2は前提条件
    ASSERT_EQ(rects.size(), 2u);
    // Standable
    EXPECT_EQ(rects[0].x, 10);
    EXPECT_EQ(rects[0].y, 20);
    EXPECT_EQ(rects[0].w, 100);
    EXPECT_EQ(rects[0].h, 50);
    // DropThrough
    EXPECT_EQ(rects[1].x, 200);
    EXPECT_EQ(rects[1].y, 300);
    EXPECT_EQ(rects[1].w, 40);
    EXPECT_EQ(rects[1].h, 20);
}

/**
 * @brief Construct a new TEST object
 * 
 * 呼び出し前にrectsに値が入っている場合の検証
 */
TEST(GameUtilTests, GameUtil_RebuildBlockRects_ClearsPreviousContents){
    // 呼び出し前にrectsに別の内容がある場合
    std::vector<Block> blocks{{0.0, 0.0, 10.0, 10.0, BlockType::Standable}};
    std::vector<SDL_Rect> rects{{999, 999, 999, 999}, {111, 111, 111, 111}};
    // rebuildBlockRects()を実施
    GameUtil::rebuildBlockRects(blocks, rects);
    // チェック
    ASSERT_EQ(rects.size(), 1u);    // 要素数が2→1になるはず
    EXPECT_EQ(rects[0].x, 0);       // 差表もチェック
}

/* ---------- GameUtil::isOutOfWorldBounds() ---------- */
// ワールドサイズ: 1600x900, マージン: 100x100

/**
 * @brief Construct a new TEST object
 * 
 * ワールド内にいるパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_InsideWorld){
    SDL_Rect r{100, 100, 20, 20};
    EXPECT_FALSE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}

/**
 * @brief Construct a new TEST object
 * 
 * ワールド上方にはみ出ているパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_AboveWorld){
    // bottom < 0 - margin_H = -100 → 画面上方へ消えた
    SDL_Rect r{100, -200, 20, 20};   // top=-200, bottom=-180
    EXPECT_TRUE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}

/**
 * @brief Construct a new TEST object
 * 
 * ワールド下方にはみ出ているパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_BelowWorld){
    // top > world_H + margin_H = 900+100 = 1000
    SDL_Rect r{100, 1100, 20, 20};
    EXPECT_TRUE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}

/**
 * @brief Construct a new TEST object
 * 
 * ワールド左方にはみ出ているパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_LeftOfWorld){
    // right < 0 - margin_W = -100 → x+w < -100
    SDL_Rect r{-200, 100, 20, 20};   // right=-180
    EXPECT_TRUE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}

/**
 * @brief Construct a new TEST object
 * 
 * ワールド右方にはみ出ているパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_RightOfWorld){
    // left > world_W + margin_W = 1600+100 = 1700
    SDL_Rect r{1800, 100, 20, 20};
    EXPECT_TRUE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}

/**
 * @brief Construct a new TEST object
 * 
 * ワールド上方に少しはみ出ているが，マージン内にとどまっているパターン
 */
TEST(GameUtilTests, GameUtil_OutOfWorldBounds_WithinMargin){
    // bottomが0より少し上だがマージン内→false(ワールド内扱い)
    SDL_Rect r{100, -50, 20, 20};    // bottom=-30, margin_H=100 → -30 > -100 なのでfalse
    EXPECT_FALSE(GameUtil::isOutOfWorldBounds(r, 1600.0, 900.0, 100.0, 100.0));
}
