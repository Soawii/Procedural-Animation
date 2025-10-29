#include "Lizard.hpp"
#include "../util/util.hpp"
#include <math.h>

Lizard::Lizard(sf::Vector2f head_start) {
    m_velocity = 600.0f;
    m_targetPoint = head_start;
    m_spine.addHead(head_start, 52.0f / 1.5f);
    float bodyWidth[] = {58, 40, 60, 68, 71, 65, 50, 28, 15, 11, 9, 7, 7};

    for (int i = 0; i < 13; i++) {
        m_spine.addPointAtTail(32.0f, 0.0f, bodyWidth[i] / 1.5f);
    }

    m_legPoints[0] = [this]() {
        const sf::Vector2f point = m_spine.m_points[3];
        float angle = m_spine.getAngle(3) - conf::constants::PI / 2;
        const sf::Vector2f anchor = point + sf::Vector2f(cos(angle), sin(angle)) * m_spine.m_circleRadiuses[3] * 0.8f;
        angle += conf::constants::PI / 4;
        const sf::Vector2f head = anchor + sf::Vector2f(cos(angle), sin(angle)) * m_legLength * 1.8f;
        return std::make_pair(anchor, head);
    };
    m_legPoints[1] = [this]() {
        const sf::Vector2f point = m_spine.m_points[3];
        float angle = m_spine.getAngle(3) + conf::constants::PI / 2;
        const sf::Vector2f anchor = point + sf::Vector2f(cos(angle), sin(angle)) * m_spine.m_circleRadiuses[3] * 0.8f;
        angle -= conf::constants::PI / 4;
        const sf::Vector2f head = anchor + sf::Vector2f(cos(angle), sin(angle)) * m_legLength * 1.8f;
        return std::make_pair(anchor, head);
    };
    m_legPoints[2] = [this]() {
        const sf::Vector2f point = m_spine.m_points[7];
        float angle = m_spine.getAngle(7) - conf::constants::PI / 2;
        const sf::Vector2f anchor = point + sf::Vector2f(cos(angle), sin(angle)) * m_spine.m_circleRadiuses[7] * 0.8f;
        angle += conf::constants::PI / 4;
        const sf::Vector2f head = anchor + sf::Vector2f(cos(angle), sin(angle)) * m_legLength * 1.8f;
        return std::make_pair(anchor, head);
    };
    m_legPoints[3] = [this]() {
        const sf::Vector2f point = m_spine.m_points[7];
        float angle = m_spine.getAngle(7) + conf::constants::PI / 2;
        const sf::Vector2f anchor = point + sf::Vector2f(cos(angle), sin(angle)) * m_spine.m_circleRadiuses[7] * 0.8f;
        angle -= conf::constants::PI / 4;
        const sf::Vector2f head = anchor + sf::Vector2f(cos(angle), sin(angle)) * m_legLength * 1.8f;
        return std::make_pair(anchor, head);
    };

    for (int i = 0; i < 4; i++) {
        m_legHeadPoints.push_back(TimingVariable<sf::Vector2f>({0.0f, 0.0f}, 0.05f, TimingFunctions::easeOutQuad));

        const auto anchor_head = m_legPoints[i]();
        m_legs[i].addHead(anchor_head.second, 12.0f);
        m_legs[i].addPointAtTail(i < 2 ? m_legLength : m_legLength, 0.0f, 12.0f);
        m_legs[i].addPointAtTail(i < 2 ? m_legLength : m_legLength, 0.0f, 12.0f);
        m_legs[i].resolveJoint(anchor_head.second, anchor_head.first);
    }
}

void Lizard::update(float dt) {
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
    for (int i = 0; i < 4; i++) {
        auto anchor_head = m_legPoints[i]();
        if (util::getLen(m_legHeadPoints[i].get() - anchor_head.first) > 2.0f * m_legLength) {
            m_legHeadPoints[i] = anchor_head.second;
        }
        m_legs[i].resolveJoint(m_legHeadPoints[i].get(), anchor_head.first);
    }
}

void Lizard::draw(sf::RenderWindow* window) {
    Entity::draw(window);

    for (int i = 0; i < 4; i++) {
        const auto& p = m_legs[i].getPoints(true);
        std::vector<sf::Vector2f> smoothPoints; 
        int starting_point = util::smoothPointsCatmull(p.first, smoothPoints, p.second, 4);
        util::drawSmooth(smoothPoints, starting_point, 5.0f, sf::Color(82, 121, 111), sf::Color::White, window);
    }

    const auto& p = m_spine.getPoints();
    std::vector<sf::Vector2f> smoothPoints; 
    int starting_point = util::smoothPointsCatmull(p.first, smoothPoints, p.second, 4);
    util::drawSmooth(smoothPoints, starting_point, 5.0f, sf::Color(82, 121, 111), sf::Color::White, window);
    
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
