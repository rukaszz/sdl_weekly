#ifndef STAGEDEFINITIONLOADER_H
#define STAGEDEFINITIONLOADER_H

#include "StageDefinition.hpp"

#include <vector>
#include <string>

class StageDefinitionLoader{
public:
    static std::vector<StageDefinition> loadStagesFromJson(const std::string& path);
};

#endif  // STAGEDEFINITIONLOADER_H
