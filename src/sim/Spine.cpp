#include "Spine.hpp"
#include "../util/util.hpp"
#include "../conf.hpp"
#include <math.h>
#include <iostream>

Spine::Spine() {}

Spine::Spine(sf::Vector2f head_point, float circle_radius) {
    addHead(head_point, circle_radius);
}

void Spine::addHead(sf::Vector2f head_point, float circle_radius) {
    m_points.push_back(head_point);
    m_circleRadiuses.push_back(circle_radius);
}

void Spine::addPointAtTail(float distance, float angle, float circle_radius) {
    const sf::Vector2f tail_point = m_points.back();
    const sf::Vector2f distance_vector = sf::Vector2f(cos(angle), sin(angle)) * distance;
    m_points.push_back(tail_point + distance_vector);
    m_distances.push_back(distance);
    m_circleRadiuses.push_back(circle_radius);
}

void Spine::resolve(sf::Vector2f head_pos) {
    if (m_points[0] == head_pos) {
        return;
    }
    m_points[0] = head_pos;
    float prev_angle = getAngle(0);
    for (int i = 1; i < m_points.size(); i++) {
        float curr_angle = getAngle(i);
        float d1 = fabs(prev_angle - curr_angle);
        float d2 = fabs(prev_angle + 2 * conf::constants::PI - curr_angle);
        float d3 = fabs(prev_angle - 2 * conf::constants::PI - curr_angle);
        float angle_diff = std::min(d1, std::min(d2, d3));
        if (angle_diff > m_maxAngle) {
            float to_move = angle_diff - m_maxAngle;
            float dir = ((d1 <= d2 && d1 <= d3) ? (prev_angle - curr_angle) : ((d2 <= d1 && d2 <= d3) ? (prev_angle + 2 * conf::constants::PI - curr_angle) : (prev_angle - 2 * conf::constants::PI - curr_angle)));
            curr_angle += to_move * (dir < 0.0f ? -1.0f : 1.0f);
        }
        prev_angle = curr_angle;
        curr_angle += conf::constants::PI;
        m_points[i] = util::projectPointOnRadiusFromAnchor(m_points[i - 1], curr_angle, m_distances[i - 1]);
    }
}

void Spine::resolveJoint(sf::Vector2f pos, sf::Vector2f anchor) {
    m_points[0] = pos;
    for (int i = 1; i < m_points.size(); i++) {
        float curr_angle = getAngle(i) + conf::constants::PI;
        m_points[i] = util::projectPointOnRadiusFromAnchor(m_points[i - 1], curr_angle, m_distances[i - 1]);
    }

    m_points[m_points.size() - 1] = anchor;
    for (int i = m_points.size() - 2; i >= 0; i--) {
        float curr_angle = getAngle(i + 1);
        m_points[i] = util::projectPointOnRadiusFromAnchor(m_points[i + 1], curr_angle, m_distances[i]);
    }
}

float Spine::getAngle(int i) {
    const sf::Vector2f tangent = (i == 0) ? m_points[0] - m_points[1] : m_points[i - 1] - m_points[i];
    return atan2f(tangent.y, tangent.x);
}

std::pair<std::vector<sf::Vector2f>, size_t> Spine::getPoints(bool double_points, int start, int end) {
    if (start = -1)
        start = 0;
    if (end == -1)
        end = m_points.size() - 1;
    const float PI = conf::constants::PI;
    std::vector<sf::Vector2f> points;

    // right side
    for (size_t i = start; i <= end; i++)
    { 
        float angle = getAngle(i);
        float a2 = angle + PI / 2.0f;
        points.push_back(m_points[i] + sf::Vector2f(std::cos(a2), std::sin(a2)) * m_circleRadiuses[i]);

        if (double_points && i > 0 && i < m_points.size() - 1) {
            angle = getAngle(i + 1);
            a2 = angle + PI / 2.0f;
            points.push_back(m_points[i] + sf::Vector2f(std::cos(a2), std::sin(a2)) * m_circleRadiuses[i]);
        }
    }

    // tail
    float angle = getAngle(end);
    points.push_back(m_points[end] + sf::Vector2f(std::cos(angle + PI - PI / 6), std::sin(angle + PI - PI / 6)) * m_circleRadiuses[end]);
    points.push_back(m_points[end] + sf::Vector2f(std::cos(angle + PI), std::sin(angle + PI)) * m_circleRadiuses[end]);
    points.push_back(m_points[end] + sf::Vector2f(std::cos(angle + PI + PI / 6), std::sin(angle + PI + PI / 6)) * m_circleRadiuses[end]);

    // left side
    for (int i = end; i >= start; i--)
    {
        if (double_points && i > 0 && i < m_points.size() - 1) {
            float angle = getAngle(i + 1);
            float a1 = angle - PI / 2.0f;
            points.push_back(m_points[i] + sf::Vector2f(std::cos(a1), std::sin(a1)) * m_circleRadiuses[i]);
        }

        float angle = getAngle(i);
        float a1 = angle - PI / 2.0f;
        points.push_back(m_points[i] + sf::Vector2f(std::cos(a1), std::sin(a1)) * m_circleRadiuses[i]);
    }

    // head
    angle = getAngle(start);
    points.push_back(m_points[start] + sf::Vector2f(std::cos(angle - PI/6), std::sin(angle - PI/6)) * m_circleRadiuses[start]);
    size_t starting_point = points.size();
    points.push_back(m_points[start] + sf::Vector2f(std::cos(angle), std::sin(angle)) * m_circleRadiuses[start]);
    points.push_back(m_points[start] + sf::Vector2f(std::cos(angle + PI/6), std::sin(angle + PI/6)) * m_circleRadiuses[start]);

    sf::Vector2f tangent2 = m_points[start + 1] - m_points[start + 2];
    float angle2 = std::atan2(tangent2.y, tangent2.x);
    points.push_back(m_points[start] + sf::Vector2f(std::cos(angle + PI/2), std::sin(angle + PI/2)) * m_circleRadiuses[start]);
    points.push_back(m_points[start + 1] + sf::Vector2f(std::cos(angle + PI/2), std::sin(angle + PI/2)) * m_circleRadiuses[start + 1]);
    points.push_back(m_points[start + 2] + sf::Vector2f(std::cos(angle2 + PI/2), std::sin(angle2 + PI/2)) * m_circleRadiuses[start + 2]);

    return std::make_pair(points, starting_point);  
}