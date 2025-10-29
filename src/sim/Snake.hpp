#pragma once
#include "Entity.hpp"
#include "Spine.hpp"

class Snake : public Entity {
public:
    Spine m_spine;

    Snake(sf::Vector2f head_start);

    void update(float dt) override;
    
    void draw(sf::RenderWindow* window) override;
};