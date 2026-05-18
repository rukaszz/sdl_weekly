#include "MusicSystem.hpp"

#include <iostream>

/**
 * @brief Construct a new Music System:: Music System object
 * 
 */
MusicSystem::MusicSystem(){}

/**
 * @brief Destroy the Music System:: Music System object
 * 
 */
MusicSystem::~MusicSystem(){
    cleanup();
}

/**
 * @brief BGMの読み込み関数
 * 
 * @param id 
 * @param path 
 */
void MusicSystem::load(MusicId id, const std::string& path){
    // 音楽ファイルの読み込み(エラー時NULLが返る)
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    // エラー発生時はcerrでストリーム出力する
    if(!music){
        std::cerr << "[MusicSystem] load failed: " << path
                  << " (" << Mix_GetError() << ")\n";
        return;
    }
    // 登録済みかチェック(登録済みの場合は上書きする)
    auto it = musics.find(id);
    // イテレータが最後まで到達しない かつ value(secondで指定)が存在するなら
    if(it != musics.end() && it->second){
        Mix_FreeMusic(it->second);  // メモリ(music)の解放
    }
    // 再度valueとしてセット
    musics[id] = music;
}

/**
 * @brief BGMを鳴らす関数
 * 
 * @param id 
 * @param loops 
 */
void MusicSystem::play(MusicId id, int loops){
    // idがmusicsに登録されているかチェック
    auto it = musics.find(id);
    // イテレータが最後まで到達 もしくは valueが存在していないなら
    if(it == musics.end() || !it->second){
        return; // 処理しない
    }
    // 当該関数が呼ばれた際は，いったん止めてから再生する
    Mix_HaltMusic();    // 音楽ストリームを停止する
    // Mix_PlayMusic()は音楽オブジェクトを再生する(エラー時は-1が返る)
    // ※Mix_PlayMusic()でloopsに-1を指定すると再生が無限ループになる
    if(Mix_PlayMusic(it->second, loops) == -1){
        std::cerr << "[MusicSystem] play failed: "
                  << " (" << Mix_GetError() << ")\n";
        return;
    }
    // 現在再生中のBGMのIDをセット
    currentMusic = id;
}

/**
 * @brief play()のラッパ関数
 * 同じBGMがすでに再生中のときにplay()で再度再生しないようにする
 * 
 * @param id 
 * @param loops 
 */
void MusicSystem::playIfChanged(MusicId id, int loops){
    // currentMusicが設定済み かつ 与えられたIDがセットされており かつ　BGMが再生中
    if(currentMusic && *currentMusic == id && Mix_PlayingMusic()){
        return; // 指定したBGMを再生中なので処理しない        
    }
    // 上記の条件を満たさなければ，idで指定したBGMを再生
    play(id, loops);
}

/**
 * @brief BGMの再生を止める
 * 
 */
void MusicSystem::stop(){
    Mix_HaltMusic();
    currentMusic.reset();   // idがセットされていない状態にする
}

/**
 * @brief 音楽をフェードアウトさせる関数
 * 
 * @param ms：ここで指定したミリ秒を書けてフェードアウトする 
 */
void MusicSystem::fadeOut(int ms){
    // BGMが再生中なら
    if(Mix_PlayingMusic()){
        // 指定のmsミリ秒でフェードアウトしている
        Mix_FadeOutMusic(ms);
    }
    // BGMは停止するのでcurrentMusicをリセット
    currentMusic.reset();
}

/**
 * @brief 再生中のBGMを一時停止する
 * 
 */
void MusicSystem::pause(){
    // BGMが再生中 かつ BGMが一時停止中であるか
    if(Mix_PlayingMusic() && !Mix_PausedMusic()){
        // BGMの一時停止(再生中のみ停止可能)
        Mix_PauseMusic();
    }
}


/**
 * @brief BGMの再生を再開する
 * 
 */
void MusicSystem::resume(){
    // BGMが一時停止中であれば
    if(Mix_PausedMusic()){
        // 音楽ストリームを一時停止から復帰させる(一時停止中でなくても呼び出してエラーなどは生じない)
        Mix_ResumeMusic();
    }
}

void MusicSystem::cleanup(){
    // BGMを停止させる
    stop();
    // musicsに割り当てられたid-musicを解放する
    for(auto& [id, music] : musics){
        if(music){
            // メモリ上の音楽オブジェクト(BGM)を解放(フェードアウト中は終わるまでブロックされる)
            Mix_FreeMusic(music);
            music = nullptr;
        }
    }
    musics.clear();
}
