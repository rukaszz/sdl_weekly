#ifndef GAMEEVENTBUFFER_H
#define GAMEEVENTBUFFER_H

#include "GameEvent.hpp"

#include <vector>
#include <utility>

class GameEventBuffer{
private:
    std::vector<GameEvent> q;
public:
    /*
    push()の素直な実装：
    void push(GameEvent e){ q.push_back(std::move(e)); }
    これでは引数が値渡しなのでコピーとムーブが1回，
    push_back(move())でムーブが1回発生するので無駄が多い
    効率的な実装(オーバロード)
    // 左辺値版：1回コピーが走る
    void push(const GameEvent& e){ q.push_back(e); }
    // 右辺値版：1回ムーブが走る
    void push(GameEvent&& e){ q.push_back(std::move(e)); }
    */
    // 上記の効率的な実装をstd::forwardでテンプレート化
    // T&&は常に右辺値(rvalue)ではないため，rvalue/lvalueで振る舞いを変えなければならない
    // 単にeを渡すとlvalueとして，move(e)ではrvalueとして強制的に渡ってしまう
    // forwardを使うと，引数がrvalueかlvalueかを判断して渡せる
    template<typename T>
    void push(T&& e){
        q.emplace_back(std::forward<T>(e));
    }
    void clear(){
        q.clear();
    }
    const std::vector<GameEvent>& items() const{
        return q;
    }
};

#endif  // GAMEEVENTBUFFER_H
