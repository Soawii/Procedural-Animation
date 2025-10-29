#pragma once
#include "Entity.hpp"
#include "Spine.hpp"
#include "../util/TimingVariable.hpp"
#include <functional>
#include <vector>

class Spider : public Entity {
public:
    sf::Vector2f m_centerPos = {0.0f, 0.0f};
    float m_radius = 30.0f;
    float m_angle = 0.0f;
    Spine m_legs[8];
    std::vector<TimingVariable<sf::Vector2f>> m_legHeadPoints;
    float m_legLength = 70.0f;
    std::function<std::pair<sf::Vector2f, sf::Vector2f>()> m_legPoints[8];

    Spider(sf::Vector2f head_start);

    void update(float dt) override;
    
    void draw(sf::RenderWindow* window) override;
};