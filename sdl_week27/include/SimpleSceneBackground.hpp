#ifndef SIMPLESCENEBACKGROUND_H
#define SIMPLESCENEBACKGROUND_H

#include "BackgroundRenderer.hpp"

#include "BackgroundId.hpp"
#include "Camera.hpp"

struct BackgroundTextureContext;
class Renderer;

/**
 * @brief Title/GameOver/Clear/Resultなどで使用する
 * カメラに追従しない簡単な背景描画クラス
 * 
 */
class SimpleSceneBackground{
private:
    BackgroundRenderer bgRenderer;
    Camera fixedCamera;
public:
    SimpleSceneBackground(int w, int h);

    void setPreset(const BackgroundTextureContext& tex, BackgroundId id);
    void render(Renderer& renderer) const;
    void clear();
};

#endif  // SIMPLESCENEBACKGROUND_H
