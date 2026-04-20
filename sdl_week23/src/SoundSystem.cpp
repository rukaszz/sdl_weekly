#include "SoundSystem.hpp"

#include <iostream>
#include <variant>

#include "GameEvent.hpp"
#include "GameEventBuffer.hpp"

/**
 * @brief Construct a new Sound System:: Sound System object
 * 
 */
SoundSystem::SoundSystem(){
    // SDL_INIT_AUDIO, Mix_OpenAudioはSdlSystem::SdlSystem()で初期化済み
}

/**
 * @brief Destroy the Sound System:: Sound System object
 * 
 */
SoundSystem::~SoundSystem(){
    cleanup();
}

/**
 * @brief WAVファイルをロードしてsoundsへ登録する
 * Mixerの初期化失敗ではゲーム終了，SEの読み込み失敗はそのSEがない状態で継続する
 * ロード失敗時はcerrで警告を出すが，例外は投げない
 * 
 * @param id 
 * @param path 
 */
void SoundSystem::load(SoundId id, const std::string& path){
    // 音声ファイルを読み込む(chunk≒メモリ)
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    // 読み込みに失敗したらエラーを返す
    if(!chunk){
        // cerrでストリーム出力する
        std::cerr << "[Sound System] load failed" << path
                  << "(" << Mix_GetError() << ")\n";
        return;
    }
    // 登録済みの場合，解放して上書きする
    auto it = sounds.find(id);  // ハッシュテーブルなのでidで探せる
    // イテレータが最後まで到達しないかつvalue(secondで指定)が存在するなら
    if(it != sounds.end() && it->second){
        Mix_FreeChunk(it->second);  // チャンク解放
    }
    // soundsへセット
    sounds[id] = chunk;
}

/**
 * @brief SoundIdに対応するSEを鳴らす
 * 未ロードのIDは無視される
 * 
 * @param id 
 */
void SoundSystem::play(SoundId id){
    // 与えられたIDに対応する値を取得
    auto it = sounds.find(id);
    // イテレータが最後まで到達 or 値がセットされていない
    if(it == sounds.end() || !it->second){
        return;
    }
    // 鳴らす
    /**
     * @brief Mix_PlayChannel
     * 
     * @param channel：再生するチャネル(-1なら空いているチャネルが選ばれる)
     * @param chunk：鳴らす音(チャンク)
     * @param loops：ループ回数(-1で無限ループ)
     * 
     */
    Mix_PlayChannel(-1, it->second, 0);
}

/**
 * @brief SEイベントの処理
 * GameEventBufferにPlaySoundEventsが入っていた場合，play()で鳴らす
 * 
 * @param events 
 */
void SoundSystem::process(GameEventBuffer& events){
    events.consumeIf(
        // Pred: PlaySoundEventのチェック
        [](const GameEvent& ev){ return std::holds_alternative<PlaySoundEvent>(ev); }, 
        // Fin: PlaySoundEventの消費
        [&](const GameEvent& ev){
            const auto& pse = std::get<PlaySoundEvent>(ev);
            play(pse.id);
        }
    );
}

/**
 * @brief 終了時に呼ばれるお片付け関数
 * chunkやMixerを終了する
 * 
 */
void SoundSystem::cleanup(){
    // chunkに登録しているSEを解放
    for(auto& [id, chunk] : sounds){
        if(chunk){
            Mix_FreeChunk(chunk);
            chunk = nullptr;
        }
    }
    sounds.clear();
}
