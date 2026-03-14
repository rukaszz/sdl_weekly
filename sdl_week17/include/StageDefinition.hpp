#ifndef STAGEDEFINITION_H
#define STAGEDEFINITION_H

#include <string>
#include <vector>

#include "GameEvent.hpp"
#include "EnemyType.hpp"

/**
 * @brief 敵の出現(読み込み)に関わる構造体
 * 
 */
struct EnemySpawn{
    double x;
    double y;
    double speed;
    EnemyType type;
};

/**
 * @brief アイテム出現(ステージ読み込み時)に関わる構造体
 * 
 */
struct ItemSpawn{
    double x;
    double y;
    ItemType type;
};

/**
 * @brief ステージ定義管理用
 * レベルファイルのパスやプレイヤーの初期位置などを持つ
 */
struct StageDefinition{
    std::string name;       // ステージ名("Stage 1"など)
    std::string levelFile;  // "assets/levels/levelN.txt"など
    double playerStart_X;   // ステージの開始位置
    double playerStart_Y;
    std::vector<EnemySpawn> enemySpawns;    // 敵出現の情報
    std::vector<ItemSpawn> itemSpawns;      // アイテム出現の情報
};

#endif  // STAGEDEFINITION_H
