# CMakeバージョン指定
cmake_minimum_required(VERSION 3.16)
# プロジェクトで要求するC++の最低バージョン
project(sdl_week03 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# build場所指定
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# pkg-configを利用
find_package(PkgConfig REQUIRED)

# SDL2
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Sprite.cpp
    src/Texture.cpp
    src/Player.cpp
    src/Game.cpp
    src/Enemy.cpp
    src/AnimationController.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
