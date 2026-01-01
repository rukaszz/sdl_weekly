#ifndef PHYSICS_H
#define PHISICS_H

#include <vector>
#include "Block.hpp"

class Player;

namespace Physics{
    void resolveVerticalBlockCollision(Player& player, 
                                       double prevFeet, 
                                       double newFeet, 
                                       const std::vector<Block>& blocks);
}

#endif  // PHISICS_H