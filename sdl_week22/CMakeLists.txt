# CMakeバージョン指定
cmake_minimum_required(VERSION 3.16)
# プロジェクトで要求するC++の最低バージョン
project(sdl_week22 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# build場所指定
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# pkg-configを利用
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json REQUIRED)

# テスト用
enable_testing()
add_subdirectory(tests)

# SDL2
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)

add_executable(${PROJECT_NAME}
    src/AnimationController.cpp
    src/Block.cpp
    src/BlockLevelLoader.cpp
    src/BlockRenderer.cpp
    src/BlockSystem.cpp
    src/BossAISystem.cpp
    src/BossEnemy.cpp
    src/Bullet.cpp
    src/Camera.cpp
    src/Character.cpp
    src/ChaserEnemy.cpp
    src/ClearScene.cpp
    src/CollisionSystem.cpp
    src/Enemy.cpp
    src/EnemyAISystem.cpp
    src/EnemyBullet.cpp
    src/FireBall.cpp
    src/Game.cpp
    src/GameOverScene.cpp 
    src/Input.cpp
    src/ItemRenderer.cpp
    src/ItemSystem.cpp
    src/JumperEnemy.cpp
    src/Physics.cpp
    src/Player.cpp
    src/PlayerEnemyCollisionUtil.cpp
    src/PlayingScene.cpp
    src/ProjectileSystem.cpp
    src/Renderer.cpp
    src/ResultScene.cpp
    src/Scene.cpp
    src/SceneControl.cpp
    src/SdlSystem.cpp
    src/SoundSystem.cpp
    src/Sprite.cpp
    src/StageDefinitionLoader.cpp
    src/Text.cpp
    src/TextTexture.cpp
    src/Texture.cpp
    src/TitleScene.cpp
    src/TurretEnemy.cpp
    src/WalkerEnemy.cpp
    src/Window.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
    ${SDL2TTF_INCLUDE_DIRS}
    ${SDL2MIXER_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
    ${SDL2TTF_LIBRARIES}
    ${SDL2MIXER_LIBRARIES}
    nlohmann_json::nlohmann_json
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -std=c++20)
