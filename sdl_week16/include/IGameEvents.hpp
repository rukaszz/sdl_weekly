#ifndef IGAMEEVENTS_H
#define IGAMEEVENTS_H

#include "GameEvent.hpp"

class IGameEvents{
public:
    virtual ~IGameEvents() = default;
    virtual void requestScene(GameScene gs) = 0;
    virtual void addScore(int delta) = 0;
};

#endif  // IGAMEEVENTS_H
