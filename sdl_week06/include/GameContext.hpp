#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <vector>
#include <random>
#include <memory>

// 前方宣言
class Renderer;
class Player;
class Enemy;
class TextTexture;
class Text;
class Input;
struct Block;

/**
 * @brief 主にプレイヤーに関わるオブジェクト(エンティティ)を管理
 * 
 */
struct EntityContext{
    Player& player;
    std::vector<std::unique_ptr<Enemy>>& enemies;
    std::vector<Block>& blocks;
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

    EntityContext entityCtx;
    TextRenderContext textRenderCtx;
    RandomContext randomCtx;
};

#endif  // GAMECONTEXT_H