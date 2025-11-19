cmake_minimum_required(VERSION 3.16)
project(build/sdl_week1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)

find_package(SDL2 REQUIRED)
add_executable(sdl_week1
    src/main.cpp
    include/Window.hpp
    src/Window.cpp
    include/Renderer.hpp
    src/Renderer.cpp
)
target_include_directories(sdl_week1 PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(sdl_week1 PRIVATE ./include)
target_link_libraries(sdl_week1 PRIVATE ${SDL2_LIBRARIES})