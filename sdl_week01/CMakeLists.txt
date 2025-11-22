cmake_minimum_required(VERSION 3.16)
project(build/sdl_week01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)

find_package(SDL2 REQUIRED)
add_executable(sdl_week01
    src/main.cpp
    include/Window.hpp
    src/Window.cpp
    include/Renderer.hpp
    src/Renderer.cpp
    include/Player.hpp
    src/Player.cpp
    include/Game.hpp
    src/Game.cpp
)
target_compile_options(sdl_week01 PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(sdl_week01 PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(sdl_week01 PRIVATE ./include)
target_link_libraries(sdl_week01 PRIVATE ${SDL2_LIBRARIES})