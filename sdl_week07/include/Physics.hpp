#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "Block.hpp"

/**
 * @brief Playerクラスのprivateなメンバ変数を渡すための構造体
 * データ転送オブジェクト(DTO)
 * 参照渡しなので，出力もnewFeet/vv/onGroundが上書きされる
 * 
 */
struct VerticalCollisionState{
    // 入力
    double prevFeet;
    double newFeet;
    double x;
    double width;
    double vv;
    bool   onGround;
};

namespace Physics{
    void resolveVerticalBlockCollision(VerticalCollisionState& vcs, const std::vector<Block>& blocks);
}

#endif  // PHYSICS_H