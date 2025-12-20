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
Action Input::actionKeyMap(SDL_Keycode k){
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
        return Action::Jump;
    case SDLK_ESCAPE:
        return Action::Pause;
    case SDLK_RETURN:
        return Action::Enter;
    default:
        return Action::None;
    }
}

/**
 * @brief 入力を検知する関数
 * 
 * @param e SDL_Event(入力を取り出す用) 
 */
void Input::handleEvent(const SDL_Event& e){
    if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
        Action act = actionKeyMap(e.key.keysym.sym);
        if(act == Action::None){
            return;
        }
        int idx = static_cast<int>(act);
        if(e.type == SDL_KEYDOWN){
            is.pressed[idx] = true;
            is.justPressed[idx] = true;
        } else {    // SDL_KEYUP
            is.pressed[idx] = false;
        }
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

/**
 * @brief デバッグ用関数
 * trueな状態のキーが標準出力へ出力される
 * 
 */
void Input::debugPrintInput(){
    int ActionCount = static_cast<int>(Action::Enter) + 1;
    for(int i = 0; i < ActionCount; ++i){
        Action a = static_cast<Action>(i);
        auto it = ActionNames.find(a);
        if(it == ActionNames.end()){
            continue;
        }
        const std::string& name = it->second;

        if(is.pressed[i]){
            std::cout << "pressed: " << name << std::endl;
        }
        if(is.justPressed[i]){
            std::cout << "justPressed: " << name << std::endl;
        }
    }
}
