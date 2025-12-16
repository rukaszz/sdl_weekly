#include "Scene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

/**
 * @brief Construct a new Scene:: Scene object
 * 
 * @param g 
 */
Scene::Scene(Game& g, const GameContext& context)
    : game(g)
    , ctx(context)
{

}


