#include "Window.hpp"
#include <stdexcept>

Window::Window(const std::string& title, int width, int height){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        throw std::runtime_error("SDL_Init failed. ");
    }
    window = SDL_CreateWindow(
        title.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        SDL_WINDOW_SHOWN
    );

    if(!window){
        SDL_Quit();
        throw std::runtime_error("SDL_CreateWindow failed. ");
    }
}

Window::~Window(){
        SDL_DestroyWindow(window);
        SDL_Quit();
}

