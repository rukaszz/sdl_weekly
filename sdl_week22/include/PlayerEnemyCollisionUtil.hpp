#ifndef PLAYERENEMYCOLLISIONUTIL_H
#define PLAYERENEMYCOLLISIONUTIL_H

#include <SDL2/SDL.h>

enum class PlayerEnemyCollisionResult{
    None,       // 処理なし
    StompEnemy, // 敵を踏んだ判定
    PlayerHit,  // 敵に当たった
};

namespace PlayerEnemyCollision{
    PlayerEnemyCollisionResult resolvePlayerEnemyCollision(
        const SDL_Rect& playerRect, 
        double playerPrevFeet, 
        double playerNewFeet, 
        double playerVv,
        const SDL_Rect& enemyRect 
    );
}   // PlayerEnemyCollision

#endif  // PLAYERENEMYCOLLISIONUTIL_H