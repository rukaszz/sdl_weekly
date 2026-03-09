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

#endif  // STAGEDEFINITION_H
