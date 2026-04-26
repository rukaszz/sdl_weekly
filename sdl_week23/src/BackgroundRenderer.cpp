#include "BackgroundRenderer.hpp"

#include <algorithm>

#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

/**
 * @brief Construct a new Background Renderer object
 * 
 * @param screen_W 
 * @param screen_H 
 */
BackgroundRenderer::BackgroundRenderer(int screen_W_, int screen_H_)
    : screen_W(screen_W_), screen_H(screen_H_)
{

}
 
/**
 * @brief レイヤ追加関数
 * privateなvectorメンバ変数layersへ追加する関数 
 * 
 * @param tex 
 * @param parallaxFactor 
 */
void BackgroundRenderer::addLayer(const Texture& tex, double parallaxFactor){
    parallaxFactor = std::clamp(parallaxFactor, 0.0, 1.0);
    layers.push_back(BackgroundLayer{tex, parallaxFactor});
}

/**
 * @brief 全てのレイヤを描画する関数
 * 
 * @param renderer 
 * @param camera
 */
void BackgroundRenderer::renderBackground(Renderer& renderer, const Camera& camera) const{
    // 各レイヤーを処理
    for(const auto& layer : layers){
        /** 
         * カメラの進む速度にparallaxFactorをかけてカメラよりゆっくりスクロールする
         * parallaxFactorが0.0なら背景は動かない(固定)
         * % screen_W による剰余算をすることで，画面内にscroll_Xが収まり，余りの背景が折り返される
         */ 
        const int scroll_X = static_cast<int>(camera.x * layer.parallaxFactor) % screen_W;
        /**
         * 背景テクスチャをscreen全体+offsetで左右へ広げる
         * dst1：現在の画面にぴったり収まる，あるいはカメラから若干スクロールが遅れている
         * dst2：少し先の背景の座標を持つ
         * もし左端が見切れる場合は，dst2が"%screen_W"で折り返される
         */
        SDL_Rect dst1{-scroll_X, 0, screen_W, screen_H};
        SDL_Rect dst2{dst1.x+screen_W, 0, screen_W, screen_H};
        // レンダラー呼び出しdst1→dst2
        renderer.drawImage(layer.texture, dst1);
        renderer.drawImage(layer.texture, dst2);
    }
}

/**
 * @brief 外部からレイヤをクリアするための関数
 * 
 */
void BackgroundRenderer::clearLayers(){
    layers.clear();
}

/**
 * @brief BackgroundDecorationへの追加
 * 
 * @param tex 
 * @param world_X 
 * @param screen_Y 
 * @param parallaxFactor 
 */
void BackgroundRenderer::addDecoration(const Texture& tex, int world_X, int screen_Y, double parallaxFactor){
    parallaxFactor = std::clamp(parallaxFactor, 0.0, 1.0);
    decorations.push_back(BackgroundDecoration{tex, world_X, screen_Y, parallaxFactor});
}
/**
 * @brief Decorationの構築
 * 画面外へ消えた場合の考慮が必要
 * 
 * @param renderer 
 * @param camera 
 */
void BackgroundRenderer::renderBgDecoration(Renderer& renderer, const Camera& camera) const{
    // decorationの分解
    for(const auto& deco : decorations){
        // X軸の視差効果
        const int screen_X = static_cast<int>(deco.world_X - camera.x * deco.parallaxFactor);
        // 画面外は描画しない
        const int tex_W = deco.texture.w;
        // 画面左へ抜けた or 画面右へ抜けた
        if(screen_X + tex_W < 0 || screen_X >= screen_W){
            continue;
        }
        // 描画用矩形構築
        const SDL_Rect dst{screen_X, deco.screen_Y, tex_W, deco.texture.h};
        // 描画
        renderer.drawImage(deco.texture, dst);
    }
}
// 削除
void BackgroundRenderer::clearDecoration(){
    decorations.clear();
}
