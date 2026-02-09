#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "Texture.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "FireBall.hpp"

#include <vector>
#include <random>
#include <memory>

// 前方宣言
class Renderer;
// class Texture;
class TextTexture;
class Text;
class Input;
struct Block;
struct Camera;
// class Player;
// class Enemy;
struct WorldInfo;

// 世界の広さ(ステージの広さ)
struct WorldInfo{
    double WorldWidth;
    double WorldHeight;
};

/**
 * @brief 主にプレイヤーに関わるオブジェクト(エンティティ)を管理
 * 
 */
struct EntityContext{
    Player& player;
    Texture& enemyTexture;
    std::vector<std::unique_ptr<Enemy>>& enemies;
    std::vector<Block>& blocks;
    Texture& fireballTexture;
    std::vector<std::unique_ptr<FireBall>>& fireballs;
};

/**
 * @brief テキストの画面描画関係のオブジェクトを管理
 * 
 */
struct TextRenderContext{
    Text& font;
    TextTexture& scoreText;
    TextTexture& fpsText;
};

/**
 * @brief 乱数関係の変数を管理
 * 
 */
struct RandomContext{
    std::mt19937& random;
    std::uniform_real_distribution<double>& distX;
    std::uniform_real_distribution<double>& distY;
    std::uniform_real_distribution<double>& distSpeed;
};

// ゲームで必要なデータ群
struct GameContext{
    Renderer& renderer;
    Input& input;
    Camera& camera;
    WorldInfo& worldInfo;

    EntityContext entityCtx;
    TextRenderContext textRenderCtx;
    RandomContext randomCtx;
};

#endif  // GAMECONTEXT_H