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
TEST(AllTest, GameUtil_TrimSpace){
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
TEST(AllTest, GameUtil_TrimBlankLine){
    // テスト用文字列
    std::string test_str = "          "; // 半角スペース10個
    // 期待値
    std::string expect_str = "";
    GameUtil::trim(test_str);
    // テスト
    EXPECT_EQ(test_str, expect_str);
}

/* ---------- GameUtil::intersects() ---------- */
/**
 * @brief Construct a new TEST object
 * 
 * 明らかに接触しているパターン
 */
TEST(AllTest, GameUtil_AABBClearlyIntersecting){
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{5, 5, 15, 15};

    EXPECT_TRUE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 明らかに接触していないパターン
 */
TEST(AllTest, GameUtil_AABBClearlyNotIntersecting){
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{20, 20, 30, 30};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}

/**
 * @brief Construct a new TEST object
 * 
 * 左右の境界で接触しているパターン
 */
TEST(AllTest, GameUtil_AABBTouchAtEdgeHorizontal){
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
TEST(AllTest, GameUtil_AABBTouchAtEdgeVertical){
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
TEST(AllTest, GameUtil_AABBTouchAtCorner){
    // 角(a右下とb左上)が接触→false
    SDL_Rect a{0, 0, 10, 10};
    SDL_Rect b{10, 10, 10, 20};

    EXPECT_FALSE(GameUtil::intersects(a, b));
}
