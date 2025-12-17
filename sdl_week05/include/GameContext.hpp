#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <vector>
#include <random>
#include <memory>

class Renderer;
class Player;
class Enemy;
class TextTexture;
class Text;

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

};

#endif  // GAMECONTEXT_H