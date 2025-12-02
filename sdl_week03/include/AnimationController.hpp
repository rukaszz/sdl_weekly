#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

/**
 * PlayerやEnemyなどのオブジェクトがアニメーション処理を持たないように分離
 * 
 */

class AnimationController{
private:
    int frame = 0;
    double timer = 0.0;
    double interval;
    int maxFrames;

public:
    AnimationController(int maxFrames, double interval);

    void update(double delta);

    int getFrame() const{
        return frame;
    }

    void reset(){
        frame = 0;
        timer = 0.0;
    }
};

#endif  // ANIMATIONCONTROLLER_H