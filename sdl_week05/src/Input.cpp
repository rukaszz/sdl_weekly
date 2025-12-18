#include "Input.hpp"

#include <SDL2/SDL.h>

/**
 * @brief Construct a new Input:: Input object
 * 
 */
Input::Input(){}

/**
 * @brief SDLのキーコードとActionのマッピング関数 
 * 
 * @param k 
 * @return Action 
 */
Action Input::actionKeyMap(SDL_KeyCode k){
    switch(k){
    case SDLK_LEFT:
        return Action::MoveLeft;
    case SDLK_RIGHT:
        return Action::MoveRight;
    case SDLK_UP:
        return Action::MoveUp;
    case SDLK_DOWN:
        return Action::MoveDown;
    case SDLK_SPACE:
        return Action::junp;
    default:
        return Action::Pause;
    }
}

/**
 * @brief 入力を検知する関数
 * 
 * @param e SDL_Event(入力を取り出す用) 
 */
void Input::handleEvent(const SDL_Event& e){
    switch(e.type){
    case SDL_KEYDOWN:
        Action act = actionKeyMap(e.key.keysym.sym);
        int idx = static_cast<int>(act);
        is.pressed[idx] = true;
        is.justPressed[idx] = true;
        break;
    case SDL_KEYUP:
        Action act = actionKeyMap(e.key.keysym.sym);
        int idx = static_cast<int>(act);
        is.pressed[idx] = false;
        is.justPressed[idx] = false;
    }
}

/**
 * @brief 毎フレームjustPressedをクリアする関数
 * 押された瞬間を検知するために，毎フレームリセットする
 * 
 */
void Input::update(){
    for(bool& flag : is.justPressed){
        flag = false;
    }
}
