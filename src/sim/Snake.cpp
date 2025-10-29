#include "Snake.hpp"
#include "../util/util.hpp"
#include <math.h>

Snake::Snake(sf::Vector2f head_start) {
    m_velocity = 600.0f;
    m_targetPoint = head_start;
    m_spine.addHead(head_start, 38.0f);
    for (int i = 0; i < 50; i++) {
        float r = 0.0f;
        if (i == 0) {
            r = 40.0f;
        }   
        else {
            r = 32.0f - i / 2.0f;
        }
        m_spine.addPointAtTail(32.0f, 0.0f, r);
    }
}

void Snake::update(float dt) {
    sf::Vector2f head_to_target = m_targetPoint - m_spine.m_points[0];
    float distance_to_target = util::getLen(head_to_target);
    if (distance_to_target < m_velocity * dt) {
        m_spine.resolve(m_targetPoint);
    }
    else {
        m_spine.resolve(m_spine.m_points[0] + head_to_target / distance_to_target * m_velocity * dt);
    }
}

void Snake::draw(sf::RenderWindow* window) {
    Entity::draw(window);

    const auto p = m_spine.getPoints();
    std::vector<sf::Vector2f> smoothPoints; 
    int starting_point = util::smoothPointsCatmull(p.first, smoothPoints, p.second, 4);
    util::drawSmooth(smoothPoints, starting_point, 5.0f, sf::Color(172, 57, 49), sf::Color::White, window);

    const sf::Vector2f h = m_spine.m_points[0];
    const float ha = m_spine.getAngle(0);
    const sf::Vector2f normal = sf::Vector2f(-sin(ha), cos(ha));
    const sf::Vector2f left_eye = h + normal * m_spine.m_circleRadiuses[0] * 0.7f, right_eye = h - normal * m_spine.m_circleRadiuses[0] * 0.7f;
    sf::CircleShape eye_shape(6.0f, 10);
    eye_shape.setOrigin(6.0f, 6.0f);
    eye_shape.setPosition(left_eye);
    window->draw(eye_shape);
    eye_shape.setPosition(right_eye);
    window->draw(eye_shape);
}
