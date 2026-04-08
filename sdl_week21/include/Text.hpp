#ifndef TEXT_H
#define TEXT_H

#include "Sprite.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

class Texture;
class Renderer;

class Text{
private:
    TTF_Font* font;

public:
    Text(const std::string& fontPath, int fontSize);
    ~Text();

    SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, int& outW, int& outH, SDL_Color color);
};

#endif  // TEXT_H