#include "ParticleSystem.hpp"

#include <vector>
#include <cstdint>

#include <SDL2/SDL.h>

#include "Particle.hpp"
#include "ParticleConfig.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

// コイン取得時のパーティクル
void ParticleSystem::spawnCoinSpark(double x, double y){
    // コイン取得時のパーティクルの設定を取得
    constexpr double ParticleMetrics& coinSpark = ParticleConfig::COINSPARK;
    // コイン取得時のパーティクルの色を取得
    constexpr SDL_Color& particleColor = ParticleConfig::YELLOW;
    // パーティクルの広がる方向を取得→正規化済みの斜め4方向
    constexpr auto& dirs = ParticleConfig::DIRECTION_4;
    // 広がるパーティクル(4方向)のそれぞれのデータを設定
    for(const auto& d : dirs){
        particles.emplace_back(
            x, 
            y, 
            d[0]*coinSpark.speed, 
            d[1]*coinSpark.speed,
            coinSpark.life,
            coinSpark.size, 
            particleColor
        );
    }
}
// 敵撃破時のパーティクル
void ParticleSystem::spawnEnemyBurst(double x, double y){
    // 敵撃破時のパーティクルの設定を取得
    constexpr double ParticleMetrics& enemyBurst = ParticleConfig::ENEMYBURST;
    // 敵撃破時のパーティクルの色を取得
    constexpr SDL_Color& particleColor = ParticleConfig::ORANGE;
    // パーティクルの広がる方向を取得→正規化済みの斜め8方向
    constexpr auto& dirs = ParticleConfig::DIRECTION_8;
    // 広がるパーティクル(8方向)のそれぞれのデータを設定
    for(const auto& d : dirs){
        particles.emplace_back(
            x, 
            y, 
            d[0]*coinSpark.speed, 
            d[1]*coinSpark.speed,
            coinSpark.life,
            coinSpark.size, 
            particleColor
        );
    }
}
// 更新関数
void ParticleSystem::update(double delta){
    // パーティクルを分解してそれぞれ更新
    for(auto& p : particles){
        p.x    += p.vx * delta;
        p.y    += p.vy * delta;
        p.life -= delta;
    }
    // remove_if-eraseのメソッドで寿命が尽きたパーティクルを削除
    auto it = std::remove_if(particles.begin(), particle.end(), 
        [](const Particle& p){ return p.life <= 0.0; }
    );
    particles.erase(it, particles.end());
}
// 描画
void ParticleSystem::render(Renderer& remderer, const Camera& camera) const{
    // パーティクルを分解してそれぞれ描画
    for(const auto& p : particles){
        // 寿命残量が少ないパーティクルは薄くしてフェードアウトさせる
        const double ratio = (p.maxlife > 0.0) ? (p.life / p.maxlife) : 0.0;
        SDL_Color drawColor = p.color;
        // アルファチャンネルの減衰
        drawColer.a = static_cast<Uint8>(255.0 * ratio);
        // パーティクル(矩形)の設定
        const SDL_Rect rect{
            static_cast<int>(p.x) - p.size/2, 
            static_cast<int>(p.x) - p.size/2, 
            p.size, 
            p.size
        };
        // drawRectで描画
        renderer.drawRect(rect, drawColor, camera);
    }
}
// クリア
void ParticleSystem::clear(){
    particles.clear();
}
