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
 * @brief 背景装飾用データ
 * ※Y軸がscreenなのは，装飾は雲や星など高さが固定されたものであるため
 * カメラのY軸移動に追従しないため，screen_Yで固定している
 * 
 */
struct BackgroundDecoration{
    const Texture& texture; // 装飾テクスチャ
    int world_X;            // カメラの影響を受けるワールド座標
    int screen_Y;           // スクリーン座標(カメラに依存しない)
    double parallaxFactor;  // 視差効果変数(X座標のみ適用する)
};

/**
 * @brief 背景描画用クラス
 * 
 */
class BackgroundRenderer{
private:
    // フルスクリーン背景レイヤ用vector(ベイク)
    std::vector<BackgroundLayer> layers;
    // 雲など背景装飾用vector
    std::vector<BackgroundDecoration> decorations;
    // スクリーンの幅と高さ
    int screen_W;
    int screen_H;
public:
    // コンストラクタ/デストラクタ
    BackgroundRenderer(int screen_W, int screen_H);
    ~BackgroundRenderer() = default;

    // フルスクリーン背景系関数
    // レイヤ追加関数
    void addLayer(const Texture& tex, double parallaxFactor = 0.0);
    // 全てのレイヤを描画する関数
    void renderBackground(Renderer& renderer, const Camera& camera) const;
    // レイヤの削除
    void clearLayers();

    // 背景装飾系関数
    // decorationへの追加
    void addDecoration(const Texture& tex, int world_X, int screen_Y, double parallaxFactor);
    // decorationの描画
    void renderBgDecoration(Renderer& renderer, const Camera& camera) const;
    // 削除
    void clearDecoration();
};

#endif  // BACKGROUNDRENDERER_H
