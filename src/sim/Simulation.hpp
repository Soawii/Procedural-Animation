#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

class Simulation {
public:
    std::vector<Entity*> m_entities;
    int m_chosen_entity = 0;

    Simulation();

    void update(float dt);
    
    void draw(sf::RenderWindow* window);
};