#ifndef MUSICID_H
#define MUSICID_H

#include <cstdint>

/**
 * @brief BGMのID管理用
 * 
 */
enum class MusicId : std::uint8_t{
    Title, 
    Playing, 
    Boss, 
    GameOver, 
    Clear, 
    Result, 
};

#endif  // MUSICID_H
