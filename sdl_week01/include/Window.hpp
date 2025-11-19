#ifndef WINDOW_H
#define WINDOW_H
#include <SDL2/SDL.h>
#include <string>

class Window{
private:
    // SDL_Window変数
    SDL_Window* window;

public:
    // コンストラクタ・デストラクタ
    Window(const std::string& title, int width, int height);
    ~Window();

    // windowオブジェクト取得用
    SDL_Window* get() const{
        return window;
    }
};

#endif // WINDOW_H