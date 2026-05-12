#ifndef STAGEDEFINITION_H
#define STAGEDEFINITION_H

#include <string>
#include <vector>

#include "BackgroundId.hpp"
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
 * @brief ボス戦に関わるデータの構造体
 * 
 */
struct BossBattleDefinition{
    bool enabled = false;
    double trigger_X = 0.0;
    double cameraMin_X = 0.0;
    double cameraMax_X = 0.0;
    double bossSpawn_X = 0.0;
    double bossSpawn_Y = 0.0;
    int hp = 30;
};

/**
 * @brief ステージ背景の装飾(雲など)に関わる構造体
 * 
 */
struct BgDecorationSpawn{
    int world_X;
    int screen_Y;
    double parallaxFactor;
    BgDecorationType type;
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
    BackgroundId backgroundId = BackgroundId::Forest;   // 背景の情報(デフォルトは森背景)
    std::vector<BgDecorationSpawn> bgDecorations;       // 背景用の装飾の情報
    std::vector<EnemySpawn> enemySpawns;    // 敵出現の情報
    std::vector<ItemSpawn> itemSpawns;      // アイテム出現の情報
    BossBattleDefinition bossBattleDef;     // ボス戦情報
};

#endif  // STAGEDEFINITION_H
