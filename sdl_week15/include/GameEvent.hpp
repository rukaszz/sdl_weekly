#ifndef GAMEEVENT_H
#define GAMEEVENT_H

// variantは継承と仮想関数を使用せずに多態性を実現するためのライブラリ
// 「宣言したどれか1つの型」を保持して取り扱える
#include <variant>

enum class GameScene;

struct RequestSceneEvent{
    GameScene scene;
};

struct AddScoreEvent{
    int delta;
};

// このGameEventはRequestSceneEvent, AddScoreEventを型安全に切り替えられる
using GameEvent = std::variant<RequestSceneEvent, AddScoreEvent>;

#endif  // GAMEEVENT_H
