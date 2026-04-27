#ifndef GAMEEVENT_H
#define GAMEEVENT_H

// variantは継承と仮想関数を使用せずに多態性を実現するためのライブラリ
// 「宣言したどれか1つの型」を保持して取り扱える
#include <variant>
// ビット幅が厳格に保証された整数型を使う
// 環境によってintのbitが変わる問題に対処するためにビット幅が固定された整数型を提供する
#include <cstdint>

#include "GameScene.hpp"

// enum class GameScene;

/**
 * @brief アイテムの種類を定義
 * 
 */
enum class ItemType : std::uint8_t{
    Coin,       // コイン
    Mushroom,   // きのこ
    FireFlower, // ファイアフラワー
};

/**
 * @brief プレイヤーの状態を定義
 * 
 */
enum class PlayerForm : std::uint8_t{
    Small,  // チビ状態
    Super,  // デカ状態
    Fire,   // ファイア状態
};

/**
 * @brief SEごとのID
 * ※SEは状態が確定したタイミングで鳴らされることに留意すること
 */
enum class SoundId : std::uint8_t{
    Jump, 
    Stomp, 
    BlockHit, 
    ItemGet, 
    Fireball, 
    Damage, 
    Coin, 
    PauseOpen, 
    PauseClose, 
};

/**
 * @brief SEを鳴らす要求イベント
 * 
 */
struct PlaySoundEvent{
    SoundId id;
};

/**
 * @brief GameSceneをリクエストする型
 * 
 */
struct RequestSceneEvent{
    GameScene scene;
};

/**
 * @brief スコアの加算用の型
 * 
 */
struct AddScoreEvent{
    int delta;
};

/**
 * @brief アイテムの出現に関する型
 * 
 */
struct SpawnItemEvent {
    ItemType type;
    double x;
    double y;
    // 速度は必要になったら追加
};

/**
 * @brief アイテムの種類を扱う型
 * 
 */
struct CollectItemEvent{
    ItemType type;
};

/**
 * @brief ブロックの(下から上の)ヒットを管理する型
 * 
 */
struct BlockHitEvent{
    std::size_t blockIndex; // このindexはblock/rectCachesのindexに対応する
};

// このGameEventはRequestSceneEvent, AddScoreEvent, ...を型安全に切り替えられる
using GameEvent = std::variant<
                        RequestSceneEvent, 
                        AddScoreEvent, 
                        SpawnItemEvent, 
                        CollectItemEvent,
                        BlockHitEvent, 
                        PlaySoundEvent
                    >;

#endif  // GAMEEVENT_H
