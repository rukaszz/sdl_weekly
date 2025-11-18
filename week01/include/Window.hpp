#ifndef WINDOW_H
#define WINDOW_H
#include <SDL2/SDL.h>
#include <string>

class Window{
private:
    SDL_Window* window;

public:
    Window(const std::string& title, int width, int height);
    ~Window();

    SDL_Window* get() const{
        return window;
    }
};

#endif // WINDOW_H