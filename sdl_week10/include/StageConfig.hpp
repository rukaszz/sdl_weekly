#ifndef STAGECONFIG_H
#define STAGECONFIG_H

#include <string>
#include <vector>

/**
 * @brief ステージ定義管理用
 * レベルファイルのパスやプレイヤーの初期位置などを持つ
 */
struct StageDefinition{
    std::string name;       // ステージ名("Stage 1"など)
    std::string levelFile;  // "assets/levels/levelN.txt"など
    double playerStart_X;   // ステージの開始位置
    double playerStart_Y;
};

namespace StageConfig{
    inline const std::vector<StageDefinition> STAGES = {
        {
            "Stage 1", 
            "assets/level/level1.txt", 
            100.0,  // playerStart_X
            700.0   // playerStart_Y
        }, 
        {
            "Stage 2", 
            "assets/level/level2.txt", 
            100.0,  // playerStart_X
            700.0   // playerStart_Y
        }, 
    };
}   // namespace StageConfig


#endif  // STAGECONFIG_H