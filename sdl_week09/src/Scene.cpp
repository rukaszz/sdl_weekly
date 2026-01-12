#include "Scene.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

/**
 * @brief Construct a new Scene:: Scene object
 * 
 * @param g 
 */
Scene::Scene(SceneControl& sc, GameContext& gc)
    : ctrl(sc)
    , ctx(gc)
{

}


