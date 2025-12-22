#ifndef TEXTTEXTURE_H
#define TEXTTEXTURE_H

#include <SDL2/SDL.h>

#include <string>

class Renderer;
class Text;

class TextTexture{
private:
    int width = 0;
    int height = 0;
    std::string currentStr;
    Text* text;
    
    SDL_Texture* tex = nullptr;
    Renderer& renderer;
    SDL_Color color;
    
public:
    TextTexture(Renderer& r, Text* t, SDL_Color c);
    ~TextTexture();
    void setText(const std::string& str);
    void draw(Renderer& r, int x, int y);
    void setAlpha(Uint8 alpha);

    int getWidth() const{
        return width;
    }
    int getHeight() const{
        return height;
    }
};

#endif  // TEXTTEXTURE_H