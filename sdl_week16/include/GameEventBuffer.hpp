#ifndef GAMEEVENTBUFFER_H
#define GAMEEVENTBUFFER_H

#include "GameEvent.hpp"
#include "IGameEvents.hpp"

#include <vector>
#include <utility>  // std::forward

/**
 * @brief イベントを詰めて消費して削除するバッファ
 * 
 */
class GameEventBuffer : public IGameEvents{
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
    /*
    template<typename T>
    void push(T&& e){
        q.emplace_back(std::forward<T>(e)); // std::forward
    }
    */
    // そもそもシーンのリクエストにclearやitemsを触れる余地を残さないように
    // IGameEventsというインターフェースを継承して公開範囲を限定させた
    // シーン遷移イベント
    void requestScene(GameScene gs) override{
        q.emplace_back(RequestSceneEvent{gs});
    }
    // スコア加算イベント
    void addScore(int delta) override{
        q.emplace_back(AddScoreEvent{delta});
    }
    // アイテム出現イベント
    void spawnItem(ItemType it, double x, double y) override{
        q.emplace_back(SpawnItemEvent{it, x, y});
    }
    // アイテム取得イベント
    void collectItem(ItemType it) override{
        q.emplace_back(CollectionItemEvent{it});
    }
    // プレイヤー形態変更イベント
    void setPlayerForm(PlayerForm pf) override{
        q.emplace_back(SetPlayerFormEvent{pf});
    }
    // ブロック接触イベント
    void hitBlock(std::size_t bi) override{
        q.emplace_back(BlockHitEvent{bi});
    }
    // イベントバッファのクリア
    void clear(){
        q.clear();
    }
    // イベントバッファの要素取り出し
    const std::vector<GameEvent>& items() const{
        return q;
    }
    /**
     * @brief 特定のイベントだけ消費して削除する処理
     * vectorのeraseはO(n^2)かかるが，この方法はO(n)で終わる
     * Predはstd::holds_alternativeを用いて取り出したい型を指定する(bool値)
     * Fnはstd::getによるムーブが発生する関数が指定される(getのムーブオーバーロード)
     * 
     * 注意：qはvariantであること
     * 途中でエラーが生じると，qの状態が不整合になる可能性がある※valueless_by_exception
     */
    template <typename Pred, typename Fn>
    void consumeIf(Pred pred, Fn fn){
        // 保持しておく要素を数える
        std::size_t out = 0;
        // vector qを1回舐める
        for(std::size_t i = 0; i < q.size(); ++i){
            // holds_alternativeで指定した型があるか確認(bool)
            if(pred(q[i])){
                // 型があればムーブしてqから取り除く(消費)
                fn(q[i]);   
            } else {
                // ムーブされた分要素の並びに空きができるので，outで詰める
                // vectorは連続したメモリ領域で取り扱われるのでq→qへのムーブは基本的には大丈夫
                q[out++] = std::move(q[i]);
            }
        }
        // 詰めた分サイズを調整する
        q.resize(out);
    }
};

#endif  // GAMEEVENTBUFFER_H
