#include "Window.hpp"
#include "Renderer.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool running = true;
    const char* title = "test";
    
    Window window(title, 400, 500);
    Renderer renderer(window.get());

    while(running){
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                // 閉じるボタン.
                running = false;
            }
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) {
                // ESCキー.
                running = false;
            }
        }
    }
}