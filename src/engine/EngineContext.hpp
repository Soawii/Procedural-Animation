#pragma once
#include <SFML/Graphics.hpp>
#include "../sim/Simulation.hpp"

class EngineContext {
public:
    sf::RenderWindow* m_window;
    Simulation* m_sim;
};