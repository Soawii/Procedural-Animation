#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

namespace util {
    float getSquaredLengthBetweenPoints(sf::Vector2f pos1, sf::Vector2f pos2);
    float getLen(sf::Vector2f vec);
    float timeFunction(const std::function<void(void)>& func);
    sf::Transform interpolateTransform(const sf::Transform& prev, const sf::Transform& curr, float alpha);
    sf::Vector2f normalize(sf::Vector2f v);
    sf::Vector2f projectPointOnRadiusFromAnchor(sf::Vector2f anchor, float angle, float radius);
    sf::Vector2f quadraticBezier(const sf::Vector2f& p1, const sf::Vector2f& p3, const sf::Vector2f& p2, float t);
    sf::Vector2f catmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, float t);
    int smoothPointsCatmull(const std::vector<sf::Vector2f>& points, std::vector<sf::Vector2f>& smoothPoints, int starting_point, int resolution);
    void drawSmooth(const std::vector<sf::Vector2f>& points, int starting_point, float outlineThickness, sf::Color fillColor, sf::Color outlineColor, sf::RenderWindow* window, bool draw_body_first = false);
    float angleDiff(float a1, float a2);
}