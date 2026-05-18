#ifndef BLOCKSYSTEM_H
#define BLOCKSYSTEM_H

#include <vector>
#include <variant>

class GameEventBuffer;
class Player;

struct Block;

class BlockSystem{
private:
    std::vector<Block>& blocks;
    Player& player;

public:
    explicit BlockSystem(std::vector<Block>& blocks_, Player& player_);

    void process(GameEventBuffer& events);
};

#endif  // BLOCKSYSTEM_H
