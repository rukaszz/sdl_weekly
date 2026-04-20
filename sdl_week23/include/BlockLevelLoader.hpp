#ifndef BLOCKLEVELLOADER_H
#define BLOCKLEVELLOADER_H

#include <vector>
#include <string>
#include "Block.hpp"

class BlockLevelLoader{
public:
    static std::vector<Block> loadFromFile(const std::string& path);
};

#endif  // BLOCKLEVELLOADER_H