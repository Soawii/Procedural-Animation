#pragma once
#include "Entity.hpp"
#include "Spine.hpp"
#include "../util/TimingVariable.hpp"
#include <functional>
#include <vector>

class ManyLegs : public Entity {
public:
    Spine m_spine;
    Spine m_legs[32];
    std::vector<TimingVariable<sf::Vector2f>> m_legHeadPoints;
    float m_legLength = 30.0f;
    std::function<std::pair<sf::Vector2f, sf::Vector2f>()> m_legPoints[32];

    ManyLegs(sf::Vector2f head_start);

    void update(float dt) override;
    
    void draw(sf::RenderWindow* window) override;
};