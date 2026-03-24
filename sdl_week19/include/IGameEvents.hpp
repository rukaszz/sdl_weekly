#ifndef IGAMEEVENTS_H
#define IGAMEEVENTS_H

#include <cstddef>
#include "GameEvent.hpp"

/**
 * @brief GameEventsBufferの一部機能のみを提供するためのインターフェイス
 * 目的はpushを晒さずにイベントの要求のみを上げること
 * 
 */
class IGameEvents{
public:
    virtual ~IGameEvents() = default;
    virtual void requestScene(GameScene gs) = 0;
    virtual void addScore(int delta) = 0;
    virtual void spawnItem(ItemType it, double x, double y) = 0;
    virtual void collectItem(ItemType it) = 0;
    //virtual void setPlayerForm(PlayerForm pf) = 0;
    virtual void hitBlock(std::size_t blockIndex) = 0;
};

#endif  // IGAMEEVENTS_H
