#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H

#include <vector>

class Renderer;
class Texture;
struct Camera;

/**
 * @brief 背景レイヤ用データ(1枚分)
 * ※parallaxFactor: 0.0で固定，1.0でカメラと等速
 * 
 */
struct BackgroundLayer{
    const Texture& texture; // 背景画像データ
    double parallaxFactor;  // 視差効果用変数
};

/**
 * @brief 背景描画用クラス
 * 
 */
class BackgroundRenderer{
private:
    // 背景レイヤ用vector
    std::vector<BackgroundLayer> layers;
    // スクリーンの幅と高さ
    int screen_W;
    int screen_H;
public:
    // コンストラクタ/デストラクタ
    BackgroundRenderer(int screen_W, int screen_H);
    ~BackgroundRenderer() = default;

    // レイヤ追加関数
    void addLayer(const Texture& tex, double parallaxFactor = 0.0);
    // 全てのレイヤを描画する関数
    void render(Renderer& renderer, const Camera& camera) const;
    // レイヤの削除
    void clearLayers();

};

#endif  // BACKGROUNDRENDERER_H
