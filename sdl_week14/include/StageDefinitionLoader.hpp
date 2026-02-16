#ifndef STAGEDEFINITIONLOADER_H
#define STAGEDEFINITIONLOADER_H

#include "StageDefinition.hpp"

#include <string>

class StageDefinitionLoader{
public:
    static StageDefinition loadFromJson(const std::string& path);

};

#endif  // STAGEDEFINITIONLOADER_H
