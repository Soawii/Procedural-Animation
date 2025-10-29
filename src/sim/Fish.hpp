#pragma once
#include "Entity.hpp"
#include "Spine.hpp"
#include "../util/TimingVariable.hpp"
#include <functional>
#include <vector>

class Fish : public Entity {
public:
    Spine m_spine;

    Fish(sf::Vector2f head_start);

    void update(float dt) override;
    
    void draw(sf::RenderWindow* window) override;
};