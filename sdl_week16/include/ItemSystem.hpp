#ifndef ITEMSYSTEM_H
#define ITEMSYSTEM_H

#include <vector>

#include "Item.hpp"
#include "GameEventBuffer.hpp"
#include "GameUtil.hpp"

class Player;

class ItemSystem{
private:
    std::vector<Item>& items;

public:
    explicit ItemSystem(std::vector<Item>& items_) : items(items_){}
    ~ItemSystem() = default;

    void onStageLoaded();
    void processSpawn(GameEventBuffer& events);
    void resolvePlayerCollition(Player& player, GameEventBuffer& events);
    void cleanup();
};

#endif  // ITEMSYSTEM_H
