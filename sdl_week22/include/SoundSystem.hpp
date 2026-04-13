#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <string>
#include <vector>
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
    std::unordered_map<SoundId, Mix_Chunk*> sounds;

public:
    void load(SoundId id, const std::string& path);
    void play(SoundId id);
    void process(GameEventBuffer& events);
    void cleanup();
};

#endif  // SOUNDSYSTEM_H
