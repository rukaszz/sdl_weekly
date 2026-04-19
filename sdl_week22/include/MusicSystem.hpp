#ifndef MUSICSYSTEM_H
#define MUSICSYSTEM_H

#include <unordered_map>
#include <optional>
#include <cstdint>
#include <string>

#include <SDL2/SDL_mixer.h>

#include "MusicId.hpp"

class MusicSystem{
private:
    // Id-BGMのハッシュテーブル
    std::unordered_map<MusicId, Mix_Music*> musics;
    // 現在かかっているBGM(読み込みに失敗する可能性があるのでoptional)
    std::optional<MusicId> currentMusic;

public:
    // 二重解放防止
    MusicSystem();
    ~MusicSystem();
    MusicSystem(const MusicSystem&) = delete;
    MusicSystem& operator=(const MusicSystem&) = delete;

    void load(MusicId id, const std::string& path);
    void play(MusicId id, int loops = -1);
    // BGMを切り替えられるなら切り替える関数
    void playIfChanged(MusicId id, int loops = -1);
    void stop();
    // 指定時間で音量を下げて消えていくフェードアウト
    void fadeOut(int ms);
    void pause();
    void resume();

private:
    void cleanup();
};

#endif  // MUSICSYSTEM_H
