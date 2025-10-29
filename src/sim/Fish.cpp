#include "Fish.hpp"
#include "../util/util.hpp"
#include <math.h>
#include <iostream>

Fish::Fish(sf::Vector2f head_start) {
    m_velocity = 600.0f;
    m_targetPoint = head_start;
    m_spine.m_maxAngle = conf::constants::PI / 6;
    m_spine.addHead(head_start, 68.0f / 1.8f);
    float bodyWidth[] = {81, 84, 83, 77, 64, 51, 38, 32, 19, 100};

    for (int i = 0; i < 10; i++) {
        m_spine.addPointAtTail(32.0f, 0.0f, bodyWidth[i] / 1.8f);
    }
}

void Fish::update(float dt) {
    sf::Vector2f head_to_target = m_targetPoint - m_spine.m_points[0];
    float distance_to_target = util::getLen(head_to_target);
    sf::Vector2f target_point;
    if (distance_to_target < m_velocity * dt) {
        target_point = m_targetPoint;
    }
    else {
        target_point = m_spine.m_points[0] + head_to_target / distance_to_target * m_velocity * dt;
    }

    m_spine.resolve(target_point);
}

void Fish::draw(sf::RenderWindow* window) {
    Entity::draw(window);

    sf::CircleShape topFin(20.0f, 30);
    topFin.setFillColor(sf::Color(129, 195, 215));
    topFin.setScale(0.9f, 2.0f);
    topFin.setOrigin(20.0f, 20.0f);
    topFin.setOutlineThickness(2.0f);
    sf::Vector2f center = m_spine.m_points[3];float angle = m_spine.getAngle(0);
    sf::Vector2f left = center + sf::Vector2f(cos(angle + conf::constants::PI / 2), sin(angle + conf::constants::PI / 2)) * m_spine.m_circleRadiuses[3];
    sf::Vector2f right = center + sf::Vector2f(cos(angle - conf::constants::PI / 2), sin(angle - conf::constants::PI / 2)) * m_spine.m_circleRadiuses[3];
    topFin.setPosition(left);
    topFin.setRotation((angle + conf::constants::PI / 2 - conf::constants::PI / 8) * 180.0f / conf::constants::PI);
    window->draw(topFin);
    topFin.setPosition(right);
    topFin.setRotation((angle + conf::constants::PI / 2 + conf::constants::PI / 8) * 180.0f / conf::constants::PI);
    window->draw(topFin);

    sf::CircleShape bottomFin(12.0f, 30);
    bottomFin.setFillColor(sf::Color(129, 195, 215));
    bottomFin.setScale(0.9f, 2.0f);
    bottomFin.setOrigin(12.0f, 12.0f);
    bottomFin.setOutlineThickness(2.0f);
    center = m_spine.m_points[7];
    angle = m_spine.getAngle(6);
    left = center + sf::Vector2f(cos(angle + conf::constants::PI / 2), sin(angle + conf::constants::PI / 2)) * m_spine.m_circleRadiuses[7];
    right = center + sf::Vector2f(cos(angle - conf::constants::PI / 2), sin(angle - conf::constants::PI / 2)) * m_spine.m_circleRadiuses[7];
    bottomFin.setPosition(left);
    bottomFin.setRotation((angle + conf::constants::PI / 2 - conf::constants::PI / 6) * 180.0f / conf::constants::PI);
    window->draw(bottomFin);
    bottomFin.setPosition(right);
    bottomFin.setRotation((angle + conf::constants::PI / 2 + conf::constants::PI / 6) * 180.0f / conf::constants::PI);
    window->draw(bottomFin);

    const auto& p = m_spine.getPoints();
    std::vector<sf::Vector2f> smoothPoints; 
    int starting_point = util::smoothPointsCatmull(p.first, smoothPoints, p.second, 4);
    util::drawSmooth(smoothPoints, starting_point, 5.0f, sf::Color(58, 124, 165), sf::Color::White, window);
    
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

    const sf::Vector2f p1 = m_spine.m_points[3];
    const sf::Vector2f p2 = m_spine.m_points[7];
    sf::Vector2f p3 = m_spine.m_points[5];
    std::vector<sf::Vector2f> spine_fin_points;
    const int spine_fin_point_amount = 20;
    for (int i = 0; i <= spine_fin_point_amount; i++) {
        const float t = (1.0f * i / spine_fin_point_amount);
        spine_fin_points.push_back(util::quadraticBezier(p1, p3, p2, t));
    }
    const float p3_angle = m_spine.getAngle(5);
    float angle_diff = util::angleDiff(m_spine.getAngle(2), m_spine.getAngle(7));
    const float fish_curvature = angle_diff / conf::constants::PI;
    p3 += fish_curvature * 40.0f * sf::Vector2f(cos(p3_angle + conf::constants::PI / 2), sin(p3_angle + conf::constants::PI / 2));
    for (int i = spine_fin_point_amount - 1; i >= 0; i--) {
        const float t = (1.0f * i / spine_fin_point_amount);
        spine_fin_points.push_back(util::quadraticBezier(p1, p3, p2, t));
    }
    util::drawSmooth(spine_fin_points, 0, 2.0f, sf::Color(129, 195, 215), sf::Color::White, window, true);
}
