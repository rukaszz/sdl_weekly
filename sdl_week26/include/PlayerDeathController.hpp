#ifndef PLAYERDEATHCONTROLLER_H
#define PLAYERDEATHCONTROLLER_H

class PlayerDeathController{
private:
    // 定数
    static constexpr double DURATION       = 1.0;   // 演出全体の秒数
    static constexpr double BLINK_INTERVAL = 0.1;   // 点滅間隔(秒)

    // メンバ
    double deathTimer   = 0.0;      // 死亡時の時間
    double blinkTimer   = 0.0;      // 点滅時間
    bool   blinkVisible = true;     // 点滅管理用真偽値
    bool   active       = false;    // 当該イベントの有効判定

public:
    // 演出の開始
    void start();
    // 更新：trueでGameOverへ遷移する
    bool update(double delta);
    // リセット関数(onEnter/onExit)
    void reset();

    // getter
    // 点滅表示の状態
    bool isVisible() const noexcept{
        return blinkVisible;
    }
};

#endif  // PLAYERDEATHCONTROLLER_H
