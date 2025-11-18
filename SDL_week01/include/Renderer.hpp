#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>

class Renderer{
private:
    SDL_Renderer* renderer;

public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear();
    void present();

    SDL_Renderer* get() const{
        return renderer;
    }
};

#endif  // RENDERER_H