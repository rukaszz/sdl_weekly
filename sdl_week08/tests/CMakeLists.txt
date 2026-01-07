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
)

# Physics のテスト
add_executable(
    PhysicsTests
    test_Physics_logic.cpp
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)
# Physicsのインクルードディレクトリ
target_include_directories(
    PhysicsTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
# テスト
target_link_libraries(PhysicsTests
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(PhysicsTests)
