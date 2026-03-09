#ifndef ITEMSYSTEM_H
#define ITEMSYSTEM_H

#include <vector>

class Renderer;
class GameEventBuffer;
class Item;
class Player;

struct Camera;


class ItemSystem{
private:
    std::vector<Item>& items;

public:
    explicit ItemSystem(std::vector<Item>& items_);
    ~ItemSystem() = default;

    void onStageLoaded();
    void processSpawn(GameEventBuffer& events);
    void resolvePlayerCollision(Player& player, GameEventBuffer& events);
    void cleanup();
    void render(Renderer& renderer, Camera& camera);
};

#endif  // ITEMSYSTEM_H
