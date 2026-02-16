#ifndef STAGECONFIG_H
#define STAGECONFIG_H

#include "StageDefinition.hpp"

namespace StageConfig{
    inline const std::vector<StageDefinition> STAGES = {
        {
            "Stage 1", 
            "assets/level/level1.txt", 
            100.0,  // playerStart_X
            600.0,  // playerStart_Y
            {// {.x = 100.0, .y = 200.0, .speed = 150.0, .type = EnemyType::Walker}, 
             {.x = 200.0, .y = 400.0, .speed = 100.0, .type = EnemyType::Walker},
             {.x = 400.0, .y = 636.0, .speed = 50.0,  .type = EnemyType::Chaser}
            }
        }, 
        {
            "Stage 2", 
            "assets/level/level2.txt", 
            100.0,  // playerStart_X
            600.0,  // playerStart_Y
            {{.x = 100.0, .y = 100.0, .speed = 50.0,  .type = EnemyType::Walker}, 
             {.x = 200.0, .y = 200.0, .speed = 80.0,  .type = EnemyType::Walker},
             {.x = 300.0, .y = 300.0, .speed = 110.0, .type = EnemyType::Walker},
             {.x = 400.0, .y = 400.0, .speed = 140.0, .type = EnemyType::Walker},
             {.x = 500.0, .y = 500.0, .speed = 170.0, .type = EnemyType::Walker},
             {.x = 600.0, .y = 600.0, .speed = 200.0, .type = EnemyType::Walker}
            }
        }, 
    };
}   // namespace StageConfig


#endif  // STAGECONFIG_H