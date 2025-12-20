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

// ゲームで必要なデータ群
struct GameContext{
    Renderer& renderer;
    Player& player;
    std::vector<std::unique_ptr<Enemy>>& enemies;

    TextTexture& scoreText;
    TextTexture& fpsText;

    std::mt19937& random;
    std::uniform_real_distribution<double>& distX;
    std::uniform_real_distribution<double>& distY;
    std::uniform_real_distribution<double>& distSpeed;

    Text* font;

    Input& input;

};

#endif  // GAMECONTEXT_H