##### GoogleTest用CmakeLists.txt #####
# GoogleTestを取得
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)

# C Runtime Libraryのバージョンを統一させる(Windows用)
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

# テスト対象ソース
set(GAME_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Physics.cpp
    ${CMAKE_SOURCE_DIR}/src/BlockLevelLoader.cpp
)

# テスト(cppファイルごとにテストスイートを作る)
add_executable(
    AllTest
    test_Physics_logic.cpp
    test_BlockLevelLoader.cpp
    test_GameUtil.cpp
    ${GAME_SOURCES}
)

# Physicsインクルードディレクトリ
target_include_directories(
    AllTest PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
# テスト
target_link_libraries(
    AllTest
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(AllTest)
