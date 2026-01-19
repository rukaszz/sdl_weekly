#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "Block.hpp"

/**
 * @brief ゲーム画面で物理的な挙動をするオブジェクトのprivateなメンバ変数を渡すための構造体
 * データ転送オブジェクト(DTO)
 * 参照渡しなので，出力もnewFeet/vv/onGroundが上書きされる
 * 
 */
struct VerticalCollisionState{
    // 入力
    double prevFeet;            // 1フレーム前のy座標
    double newFeet;             // 入力受け取り後のy座標
    double x;                   // x座標
    double width;               // spriteのwidth
    double vv;                  // 落下速度
    bool   onGround;            // 接地しているか
    bool   ignoreDropThrough;   // DropThroughの衝突を無視するかのフラグ
};

namespace Physics{
    void resolveVerticalBlockCollision(VerticalCollisionState& vcs, const std::vector<Block>& blocks);
}

#endif  // PHYSICS_H