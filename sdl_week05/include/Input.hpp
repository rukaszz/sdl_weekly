#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <unordered_map>

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
    Enter,
    Count,  // 配列サイズ用    
    None,   // 使用禁止
};

/**
 * @brief 入力状態のマッピング
 * 入力状態をテーブルのように管理する
 * enumの最大値(None) + 1を用いて配列を確保して，入力状態を管理する
 * 
 */
struct InputState{
    bool pressed[(int)Action::Count + 1] = {false};
    bool justPressed[(int)Action::Count + 1] = {false};
};

static const std::unordered_map<Action, std::string> ActionNames{
    {Action::MoveLeft, "MoveLeft"},
    {Action::MoveRight, "MoveRight"},
    {Action::MoveUp, "MoveUp"},
    {Action::MoveDown, "MoveDown"},
    {Action::Jump, "Jump"},
    {Action::Pause, "Pause"},
    {Action::Enter, "Enter"},
    {Action::None, "None"},
};

class Input{
private:
    InputState is;

public:
    Input();

    Action actionKeyMap(SDL_Keycode k);
    void handleEvent(const SDL_Event& e);
    void update();

    // debug用
    void debugPrintInput();

    const InputState& getState() const{
        return is;
    }
};


#endif  // INPUT_H
