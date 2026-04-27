#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <string>
#include <unordered_map>
#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

enum class SoundId : std::uint8_t;
class GameEventBuffer;

/**
 * @brief サウンド関係のイベントを管理するシステム
 * 責務は限定的
 * SDL_mixerがほとんど処理するイメージ
 * 
 */
class SoundSystem{
private:
    // SEのIDと音声チャンクのハッシュテーブル
    std::unordered_map<SoundId, Mix_Chunk*> sounds;

public:
    SoundSystem();
    ~SoundSystem();
    // Mix_Chunkの二重解放防止のためのコンストラクタ宣言
    SoundSystem(const SoundSystem&) = delete;
    SoundSystem& operator=(const SoundSystem&) = delete;
    
    void load(SoundId id, const std::string& path);
    void play(SoundId id);
    void process(GameEventBuffer& events);

private:
    void cleanup();
};

#endif  // SOUNDSYSTEM_H
