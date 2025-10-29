#include <SFML/Graphics.hpp>
#include "util.hpp"
#include <chrono>
#include <cmath>
#include <array>
#include "../conf.hpp"

namespace util {
    float getSquaredLengthBetweenPoints(sf::Vector2f pos1, sf::Vector2f pos2) {
        const sf::Vector2f diff = pos2 - pos1;
        return diff.x * diff.x + diff.y * diff.y;
    }

    float getLen(sf::Vector2f vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    float timeFunction(const std::function<void(void)>& func) {
        auto start = std::chrono::steady_clock::now();
        func();
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<float, std::micro>(end - start).count();
    }

    sf::Transform interpolateTransform(const sf::Transform& prev, const sf::Transform& curr, float alpha) {
        const float* pPrev = prev.getMatrix();
        const float* pCurr = curr.getMatrix();

        sf::Transform result;
        float m[16];
        for (int i = 0; i < 16; ++i) {
            m[i] = pPrev[i] + (pCurr[i] - pPrev[i]) * alpha;
        }
        result = sf::Transform(
            m[0], m[4], m[12],
            m[1], m[5], m[13],
            0.f, 0.f, 1.f
        );
        return result;
    }

    sf::Vector2f normalize(sf::Vector2f v) {
        return v / getLen(v);
    }

    sf::Vector2f projectPointOnRadiusFromAnchor(sf::Vector2f anchor, float angle, float radius) {
        return anchor + sf::Vector2f(cos(angle), sin(angle)) * radius;
    }

    sf::Vector2f quadraticBezier(const sf::Vector2f& p1, const sf::Vector2f& p3, const sf::Vector2f& p2, float t) {
        float u = 1 - t;
        return u*u*p1 + 2*u*t*p3 + t*t*p2;
    }

    sf::Vector2f catmullRom(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
            (2.f * p1) +
            (-p0 + p2) * t +
            (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 +
            (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3
        );
    }

    int smoothPointsCatmull(const std::vector<sf::Vector2f>& points, std::vector<sf::Vector2f>& smoothPoints, int starting_point, int resolution) {
        if (points.size() < 4)
            return 0;

        for (size_t i = 0; i < points.size() - 3; ++i) {
            for (int j = 1; j <= resolution; ++j) {
                float t = static_cast<float>(j) / resolution;
                smoothPoints.push_back(catmullRom(points[i], points[i + 1], points[i + 2], points[i + 3], t));
            }
            if (int(i + 2) == starting_point) {
                starting_point = smoothPoints.size() - 1;
            }
        }
        smoothPoints.push_back(smoothPoints[0]);
        smoothPoints.push_back(smoothPoints[1]);
        return starting_point;
    }

    void drawSmooth(const std::vector<sf::Vector2f>& points, int starting_point, float outlineThickness, sf::Color fillColor, sf::Color outlineColor, sf::RenderWindow* window, bool draw_body_first) {
        if (!window)
            return;

        // triangulate
        const int size = points.size();
        int left = starting_point - 1, right = starting_point + 1;
        bool left_turn = true;
        sf::VertexArray inside(sf::TriangleStrip);

        if (left < 0) left = size - 1;
        if (left >= size) left = 0;
        if (right < 0) right = size - 1;
        if (right >= size) right = 0;
        inside.append(points[starting_point]);
        inside.append(points[left]);
        inside.append(points[right]);
        while (left != right) {
            if (left_turn) left--;
            else right++;

            if (left < 0) left = size - 1;
            if (left >= size) left = 0;
            if (right < 0) right = size - 1;
            if (right >= size) right = 0;

            if (left == right)
                break;

            if (left_turn) inside.append(points[left]);
            else inside.append(points[right]);

            left_turn = !left_turn;
        }
        for (int i = 0; i < inside.getVertexCount(); i++) {
            inside[i].color = fillColor;
        }

        if (draw_body_first)
            window->draw(inside);
        if (outlineThickness > 0.001f) {
            sf::VertexArray outside(sf::TriangleStrip, points.size() * 2 + 2);
            for (int i = 0; i < points.size(); i++) {
                sf::Vector2f d = (i == 0) ? points[i] - points[i + 1] : (i == points.size() - 1) ? points[i - 1] - points[i] : points[i - 1] - points[i + 1];
                const float len = std::sqrt(d.x*d.x + d.y*d.y);
                d /= len;
                const sf::Vector2f normal(-d.y, d.x);
                outside[2 * i].position = points[i];
                outside[2 * i + 1].position = points[i] + normal * (outlineThickness * 0.5f);
                outside[2 * i].color = outlineColor;
                outside[2 * i + 1].color = outlineColor;
            }
            outside[outside.getVertexCount() - 2] = outside[1];
            outside[outside.getVertexCount() - 1] = outside[0];
            window->draw(outside);
        }
        if (!draw_body_first)
            window->draw(inside);
    }

    float angleDiff(float a1, float a2) {
        float d1 = fabs(a1 - a2);
        float d2 = fabs(a1 + 2 * conf::constants::PI - a2);
        float d3 = fabs(a1 - 2 * conf::constants::PI - a2);
        if (d1 <= d2 && d1 <= d3) {
            return a1 - a2;
        }
        if (d3 <= d1 && d3 <= d2) {
            return a1 - 2 * conf::constants::PI - a2;
        }
        return a1 + 2 * conf::constants::PI - a2;
    }
}