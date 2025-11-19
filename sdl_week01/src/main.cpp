#include "Window.hpp"
#include "Renderer.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

/*
* 最小限の処理を記述するmain関数
* ここはGameを呼ぶだけ
*/
int main(int argc, char* argv[]) {
    Game game;
    game.run();
}