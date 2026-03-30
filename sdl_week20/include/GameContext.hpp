#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <vector>
#include <random>
#include <memory>

#include <SDL2/SDL.h>

// #include "Texture.hpp"
// #include "Player.hpp"
// #include "Enemy.hpp"
// #include "FireBall.hpp"
// #include "EnemyBullet.hpp"
// #include "WorldInfo.hpp"

// 前方宣言
class Renderer;
class Texture;
class TextTexture;
class Text;
class Input;
class Player;
class Enemy;
class BossEnemy;
class FireBall;
class EnemyBullet;
class GameEventBuffer;
class IGameEvents;
class Item;

struct Block;
struct Camera;
struct WorldInfo;


/**
 * @brief 主にプレイヤーに関わるオブジェクト(エンティティ)を管理
 * 
 */
struct EntityContext{
    Player& player;
    Texture& enemyTexture;
    std::vector<std::unique_ptr<Enemy>>& enemies;
    BossEnemy& boss;
    std::vector<Block>& blocks;
    std::vector<SDL_Rect>& blockRectCaches;    // ブロックの矩形キャッシュ用
    std::vector<Item>& items;
    Texture& fireballTexture;
    std::vector<std::unique_ptr<FireBall>>& fireballs;
    Texture& enemyBulletTexture;
    std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets;
};

// /**
//  * @brief ブロックのテクスチャを管理
//  * 
//  */
// struct BlockTextureContext{
//     Texture& standable;
//     Texture& question;
//     Texture& usedQuestion;
//     Texture& breakable;
//     Texture& dropThrough;
//     Texture& damage;
//     Texture& Clear;
// };

// /**
//  * @brief アイテムのテクスチャを管理
//  * 
//  */
// struct ItemTextureContext{
//     Texture& coin;
//     Texture& mushroom;
//     Texture& fireFlower;
// };

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
    GameEventBuffer& eventBuffer;   // イベント消費用
    IGameEvents& events;            // イベント発行用

    EntityContext entityCtx;
    TextRenderContext textRenderCtx;
    RandomContext randomCtx;
};

#endif  // GAMECONTEXT_H
