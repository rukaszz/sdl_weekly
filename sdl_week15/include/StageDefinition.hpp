#ifndef STAGEDEFINITION_H
#define STAGEDEFINITION_H

#include <string>
#include <vector>

/**
 * @brief 敵のタイプ
 * 
 */
enum class EnemyType{
    Walker, 
    Chaser, 
    Jumper, 
    Turret, 
};

/**
 * @brief 敵の出現(読み込み)に関わる変数
 * 
 */
struct EnemySpawn{
    double x;
    double y;
    double speed;
    EnemyType type;
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
    std::vector<EnemySpawn> enemySpawns;
};

/**
 * @brief 敵のスポーン時の設定管理用
 * JsonStageDefinitionへ渡る
 * 
 */
struct JsonEnemySpawn{
    double x;
    double y;
    double speed;
    std::string type; // "walker" / "chaser" / ...
};

/**
 * @brief ステージ構成に関する定義間利用
 * ここで組み立ててStageDefinitionへ渡す
 * 
 */
struct JsonStageDefinition{
    std::string name;
    std::string levelFile;
    double playerStart_X;
    double playerStart_Y;
    std::vector<JsonEnemySpawn> enemies;
};

#endif  // STAGEDEFINITION_H
