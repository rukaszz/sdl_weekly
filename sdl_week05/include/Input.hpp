#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

// 入力テーブル
/**
 * @brief 入力に対する行動
 * 入力の状態
 * 
 */
enum class Action{
    MoveLeft, 
    MoveRight, 
    MoveUp, 
    MoveDown,
    Jump, 
    Pause,  
};

/**
 * @brief 入力状態のマッピング
 * 入力状態をテーブルのように管理する
 * enumの最大値(Pause) + 1を用いて配列を確保して，入力状態を管理する
 * 
 */
struct InputState{
    bool pressed[(int)Action::Pause + 1] = {false};
    bool justPressed[(int)Action::Pause + 1] = {false};
};

class Input{
private:
    InputState is;

public:
    Input();

    Action actionKeyMap(SDL_KeyCode k);
    void handleEvent(const SDL_Event& e);
    void update();

    const InputState& getState() const{
        return is;
    }
};


#endif  // INPUT_H
