#include "Renderer.hpp"
#include <stdexcept>

Renderer::Renderer(SDL_Window* window){
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        throw std::runtime_error("SDL_CreateRenderer failed. ");
    }
}

Renderer::~Renderer(){
    SDL_DestroyRenderer(renderer);
}

void Renderer::clear(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present(){
    SDL_RenderPresent(renderer);
}