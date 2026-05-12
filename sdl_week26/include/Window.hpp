#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <string>

class Window{
private:
    // SDL_Window変数
    SDL_Window* window = nullptr;

public:
    // コンストラクタ・デストラクタ
    Window(const std::string& title, int width, int height);
    ~Window();

    // windowサイズ返却
    SDL_Point getWindowSize() const;

    // windowオブジェクト取得用
    SDL_Window* get() const{
        return window;
    }
};

#endif // WINDOW_H