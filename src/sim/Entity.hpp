#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    sf::Vector2f m_targetPoint;
    float m_velocity;

    Entity();

    virtual void update(float dt);
    
    virtual void draw(sf::RenderWindow* window);
};