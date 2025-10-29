#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../conf.hpp"

class Spine {
public:
    std::vector<sf::Vector2f> m_points;
    std::vector<float> m_distances;
    std::vector<float> m_circleRadiuses;
    float m_maxAngle = conf::constants::PI / 5;

    Spine();
    Spine(sf::Vector2f head_point, float circle_radius);

    float getAngle(int i);

    void addHead(sf::Vector2f head_point, float circle_radius);

    void addPointAtTail(float distance, float angle, float circle_radius);

    void resolve(sf::Vector2f head_point); 
    void resolveJoint(sf::Vector2f head_point, sf::Vector2f anchor);

    std::pair<std::vector<sf::Vector2f>, size_t> getPoints(bool double_points = false, int start = -1, int end = -1);
};