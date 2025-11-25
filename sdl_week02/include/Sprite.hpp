#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

class Texture;
class Renderer;

class Sprite{
private:
    Texture& texture;
    SDL_Rect src;
    SDL_Rect dst;

public:
    Sprite(Texture& tex);

    void setPosition(int x, int y);
    void draw(Renderer& renderer);
};

#endif  // SPRITE_H