#ifndef AISTATEMACHINE_H
#define AISTATEMACHINE_H

enum class AIState{
    Walker, 
    Chaser, 
    Jumper, 
    Turret, 
    None, 
};

/**
 * @brief Enemyに持たせる状態マシン
 * 
 * Walker→Enemy→Character
 * Walker内部でAISMを持っておき，AIとしての振る舞い(例：歩くだけなど)を管理する
 * Enemyは物理/AIの2つの振る舞いを持つ
 * 
 */
class AIStateMachine{
private:
    AIState ais = AIState::Walker;
public:
    AIStateMachine();
    ~AIStateMachine() = default;
    // 振る舞い系メソッド
    virtual void update(double delta) = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    // getter/setter
    AIState getAIState() const{
        return ais;
    }
};

#endif  // AISTATEMACHINE_H
