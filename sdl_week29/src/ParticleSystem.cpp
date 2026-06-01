#include "ParticleSystem.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include <SDL2/SDL.h>

#include "Particle.hpp"
#include "ParticleConfig.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

/**
 * @brief コイン取得時のパーティクル
 * 
 * パーティクル出現位置
 * @param x 
 * @param y 
 */
void ParticleSystem::spawnCoinSpark(double x, double y){
    // パーティクルの数チェック
    if(!canSpawn(ParticleConfig::DIRECTION_4.size())){
        return;
    }
    // コイン取得時のパーティクルの設定を取得
    constexpr ParticleConfig::ParticleMetrics coinSpark = ParticleConfig::COIN_SPARK;
    // コイン取得時のパーティクルの色を取得
    constexpr SDL_Color particleColor = ParticleConfig::YELLOW;
    // パーティクルの広がる方向を取得→正規化済みの斜め4方向
    // 広がるパーティクル(4方向)のそれぞれのデータを設定
    for(const auto& dirs : ParticleConfig::DIRECTION_4){
        particles.push_back(Particle{
            x, 
            y, 
            dirs[0]*coinSpark.speed, 
            dirs[1]*coinSpark.speed,
            coinSpark.life, 
            coinSpark.life, 
            coinSpark.size, 
            particleColor
        });
    }
}

/**
 * @brief 敵撃破時のパーティクル
 * 
 * パーティクル出現位置
 * @param x 
 * @param y 
 */
void ParticleSystem::spawnEnemyBurst(double x, double y){
    // パーティクルの数チェック
    if(!canSpawn(ParticleConfig::DIRECTION_8.size())){
        return;
    }
    // 敵撃破時のパーティクルの設定を取得
    constexpr ParticleConfig::ParticleMetrics enemyBurst = ParticleConfig::ENEMY_BURST;
    // 敵撃破時のパーティクルの色を取得
    constexpr SDL_Color particleColor = ParticleConfig::ORANGE;
    // パーティクルの広がる方向を取得→正規化済みの斜め8方向
    // 広がるパーティクル(8方向)のそれぞれのデータを設定
    for(const auto& dirs : ParticleConfig::DIRECTION_8){
        particles.push_back(Particle{
            x, 
            y, 
            dirs[0]*enemyBurst.speed, 
            dirs[1]*enemyBurst.speed,
            enemyBurst.life,
            enemyBurst.life, 
            enemyBurst.size, 
            particleColor
        });
    }
}

/**
 * @brief プレイヤー死亡時のパーティクル
 * ※他より少し大きく早い
 * 出現位置
 * @param x 
 * @param y 
 */
void ParticleSystem::spawnPlayerDeath(double x, double y){
    // パーティクルの数チェック
    if(!canSpawn(ParticleConfig::DIRECTION_8.size())){
        return;
    }
    // 敵撃破時のパーティクルの設定を取得
    constexpr ParticleConfig::ParticleMetrics playerDeath = ParticleConfig::PLAYER_DEATH;
    // 敵撃破時のパーティクルの色を取得
    constexpr SDL_Color particleColor = ParticleConfig::RED;
    // パーティクルの広がる方向を取得→正規化済みの斜め8方向
    // 広がるパーティクル(8方向)のそれぞれのデータを設定
    for(const auto& dirs : ParticleConfig::DIRECTION_8){
        particles.push_back(Particle{
            x, 
            y, 
            dirs[0]*playerDeath.speed, 
            dirs[1]*playerDeath.speed,
            playerDeath.life,
            playerDeath.life, 
            playerDeath.size, 
            particleColor
        });
    }
}

/**
 * @brief ブロック破壊時のパーティクル
 * ※他より少し大きく早い
 * 出現位置
 * @param x 
 * @param y 
 */
void ParticleSystem::spawnBlockDebris(double x, double y){
    // パーティクルの数チェック
    if(!canSpawn(ParticleConfig::DIRECTION_4.size())){
        return;
    }
    // 敵撃破時のパーティクルの設定を取得
    constexpr ParticleConfig::ParticleMetrics blockDebri = ParticleConfig::BLOCK_DEBRIS;
    // 敵撃破時のパーティクルの色を取得
    constexpr SDL_Color particleColor = ParticleConfig::BROWN;
    // パーティクルの広がる方向を取得→正規化済みの斜め8方向
    // 広がるパーティクル(8方向)のそれぞれのデータを設定
    for(const auto& dirs : ParticleConfig::DIRECTION_4){
        particles.push_back(Particle{
            x, 
            y, 
            dirs[0]*blockDebri.speed, 
            dirs[1]*blockDebri.speed,
            blockDebri.life,
            blockDebri.life, 
            blockDebri.size, 
            particleColor
        });
    }
}

/**
 * @brief 更新関数
 * 
 * @param delta 
 */
void ParticleSystem::update(double delta){
    // パーティクルを分解してそれぞれ更新
    for(auto& p : particles){
        p.x    += p.vx * delta;
        p.y    += p.vy * delta;
        p.life -= delta;        // 寿命の減衰
    }
    // remove_if-eraseのメソッドで寿命が尽きたパーティクルを削除
    auto it = std::remove_if(
        particles.begin(), 
        particles.end(), 
        [](const Particle& p){
            return p.life <= 0.0;
        }
    );
    particles.erase(it, particles.end());
}

/**
 * @brief 描画関数
 * 
 * @param renderer 
 * @param camera 
 */
void ParticleSystem::render(Renderer& renderer, const Camera& camera) const{
    // パーティクルを分解してそれぞれ描画
    for(const auto& p : particles){
        // 寿命残量が少ないパーティクルは薄くしてフェードアウトさせる
        const double ratio = (p.maxLife > 0.0)
            ? std::clamp((p.life / p.maxLife), 0.0, 1.0) : 0.0;
        SDL_Color drawColor = p.color;
        // アルファチャンネルの減衰
        drawColor.a = static_cast<Uint8>(255.0 * ratio);
        // パーティクル(矩形)の設定
        const SDL_Rect rect{
            static_cast<int>(p.x) - p.size/2, 
            static_cast<int>(p.y) - p.size/2, 
            p.size, 
            p.size
        };
        // drawRectで描画
        renderer.drawRect(rect, drawColor, camera);
    }
}

/**
 * @brief パーティクル管理用コンテナクリア関数
 * 
 */
void ParticleSystem::clear(){
    particles.clear();
}

/**
 * @brief パーティクルの出現条件チェック
 * 
 * @param count 
 * @return true 
 * @return false 
 */
bool ParticleSystem::canSpawn(std::size_t count) const{
    return particles.size() + count <= ParticleConfig::MAX_PARTICLES;
}
