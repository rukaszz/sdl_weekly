#ifndef GAMEEVENTBUFFER_H
#define GAMEEVENTBUFFER_H

#include "GameEvent.hpp"

#include <vector>

class GameEventBuffer{
private:
    std::vector<GameEvent> q;
public:
    void push(GameEvent e){
        q.push_back(std::move(e));
    }
    void clear(){
        q.clear();
    }
    const std::vector<GameEvent>& items() const{
        return q;
    }
};

#endif  // GAMEEVENTBUFFER_H
