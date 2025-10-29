#include "Spider.hpp"
#include "../util/util.hpp"
#include <math.h>

Spider::Spider(sf::Vector2f head_start) {
    m_velocity = 600.0f;
    m_targetPoint = head_start;
    m_radius = 50.0f;
    m_centerPos = head_start;

    for (int i = 0; i < 8; i++) {
        m_legPoints[i] = [this, i]() {
            const sf::Vector2f point = m_centerPos;
            float angle = 2.0f * conf::constants::PI / 8 * i;
            const sf::Vector2f anchor = point + sf::Vector2f(cos(angle), sin(angle)) * m_radius * 0.8f;
            const sf::Vector2f head = anchor + sf::Vector2f(cos(angle), sin(angle)) * m_legLength * 2.0f;
            return std::make_pair(anchor, head);
        };
    }

    for (int i = 0; i < 8; i++) {
        m_legHeadPoints.push_back(TimingVariable<sf::Vector2f>({0.0f, 0.0f}, 0.05f, TimingFunctions::easeOutQuad));

        const auto anchor_head = m_legPoints[i]();
        m_legs[i].addHead(anchor_head.second, 6.0f);
        m_legs[i].addPointAtTail(m_legLength, 0.0f, 6.0f);
        m_legs[i].addPointAtTail(m_legLength, 0.0f, 6.0f);
        m_legs[i].addPointAtTail(m_legLength, 0.0f, 6.0f);
        m_legs[i].resolveJoint(anchor_head.second, anchor_head.first);
    }
}

void Spider::update(float dt) {
    sf::Vector2f head_to_target = m_targetPoint - m_centerPos;
    float distance_to_target = util::getLen(head_to_target);
    if (distance_to_target > 1.0f) {
        m_angle = atan2f(head_to_target.y, head_to_target.x);
    }
    sf::Vector2f target_point;
    if (distance_to_target < m_velocity * dt) {
        target_point = m_targetPoint;
    }
    else {
        target_point = m_centerPos + head_to_target / distance_to_target * m_velocity * dt;
    }

    m_centerPos = target_point;
    for (int i = 0; i < 8; i++) {
        auto anchor_head = m_legPoints[i]();
        if (util::getLen(m_legHeadPoints[i].getActual() - anchor_head.second) > (2.0f * m_legLength) ||
            util::getLen(m_legHeadPoints[i].getActual() - anchor_head.first) > (3.0f * m_legLength)) {
            m_legHeadPoints[i] = anchor_head.second;
        }
        m_legs[i].resolveJoint(m_legHeadPoints[i].get(), anchor_head.first);
    }
}

void Spider::draw(sf::RenderWindow* window) {
    Entity::draw(window);

    for (int i = 0; i < 8; i++) {
        const auto& p = m_legs[i].getPoints(true);
        std::vector<sf::Vector2f> smoothPoints; 
        int starting_point = util::smoothPointsCatmull(p.first, smoothPoints, p.second, 4);
        util::drawSmooth(smoothPoints, starting_point, 5.0f, sf::Color(0, 0, 0), sf::Color::White, window);
    }

    sf::CircleShape circle(m_radius, 30);
    circle.setOrigin(m_radius, m_radius);
    circle.setFillColor(sf::Color(0,0,0));
    circle.setPosition(m_centerPos);
    circle.setOutlineThickness(3.0f);
    window->draw(circle);
    
    const sf::Vector2f pointing = sf::Vector2f(cos(m_angle), sin(m_angle));
    const sf::Vector2f normal = sf::Vector2f(-pointing.y, pointing.x);
    const sf::Vector2f left_eye = m_centerPos + pointing * m_radius * 0.5f + normal * m_radius * 0.2f;
    const sf::Vector2f right_eye = m_centerPos + pointing * m_radius * 0.5f - normal * m_radius * 0.2f;
    sf::CircleShape eye_shape(6.0f, 10);
    eye_shape.setOrigin(6.0f, 6.0f);
    eye_shape.setPosition(left_eye);
    window->draw(eye_shape);
    eye_shape.setPosition(right_eye);
    window->draw(eye_shape);
}
