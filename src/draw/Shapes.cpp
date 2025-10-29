#include "Shapes.hpp"
#include "../conf.hpp"
#include "../util/util.hpp"
#include <math.h>

namespace shapes {
    void bindTexture(TextureArray& arr, sf::Texture* texture) {
        const sf::Vector2u texSize = texture->getSize();
        float minX = 100000.0f, minY = 100000.0f, maxX = -100000.0f, maxY = -100000.0f;
        const size_t N = arr.arr.getVertexCount();

        for (size_t i = 0; i < N; i++) {
            minX = std::min(minX, arr.arr[i].position.x);
            minY = std::min(minY, arr.arr[i].position.y);
            maxX = std::max(maxX, arr.arr[i].position.x);
            maxY = std::max(maxY, arr.arr[i].position.y);
        }
        for (size_t i = 0; i < N; i++) {
            float u = (arr.arr[i].position.x - minX) / (maxX - minX);
            float v = (arr.arr[i].position.y - minY) / (maxY - minY);
            arr.arr[i].texCoords = { u * texSize.x, v * texSize.y };
        }
        arr.texture = texture;
    }
    void bindTexture(TextureArray& arr, sf::Texture* texture, const sf::Vector2f pos, const sf::Vector2f size) {
        const sf::Vector2u texSize = texture->getSize();
        const float minX = pos.x, minY = pos.y, maxX = pos.x + size.x, maxY = pos.y + size.y;
        const size_t N = arr.arr.getVertexCount();

        for (size_t i = 0; i < N; i++) {
            float u = (arr.arr[i].position.x - minX) / (maxX - minX);
            float v = (arr.arr[i].position.y - minY) / (maxY - minY);
            arr.arr[i].texCoords = { u * texSize.x, v * texSize.y };
        }
        arr.texture = texture;
    }

    Line::Line(sf::Vector2f start, sf::Vector2f end, sf::Color color) : m_arr(sf::Lines, 2) {
        m_arr.arr.setPrimitiveType(sf::Lines);
        m_arr.arr.resize(2);
        m_arr.arr[0].position = start;
        m_arr.arr[1].position = end;
        m_arr.arr[0].color = color;
        m_arr.arr[1].color = color;
    }
    void Line::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    Triangle::Triangle(sf::Vector2f tip, float length, float width, float angleRadians, sf::Color color)
    : m_arr(sf::TriangleFan, 0)
    {
        angleRadians = angleRadians * 180.0f / conf::constants::PI;
        bool pointingRight = (angleRadians < 90.0f || angleRadians > 270.0f);
    
        // Three corners of the triangle
        sf::Vector2f p1 = tip; // The tip
        sf::Vector2f p2, p3;

        if (pointingRight) {
            // Tip points right, base is vertical on the left
            p2 = tip + sf::Vector2f(-length, -width / 2.0f); // Top of base
            p3 = tip + sf::Vector2f(-length, width / 2.0f);  // Bottom of base
        } else {
            // Tip points left, base is vertical on the right
            p2 = tip + sf::Vector2f(length, -width / 2.0f); // Top of base
            p3 = tip + sf::Vector2f(length, width / 2.0f);  // Bottom of base
        }
    
        m_arr.arr.resize(3);
        m_arr.arr[0] = sf::Vertex(p1, color);
        m_arr.arr[1] = sf::Vertex(p2, color);
        m_arr.arr[2] = sf::Vertex(p3, color);
    }

    void Triangle::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    Circle::Circle(sf::Vector2f center, float radius, int points, sf::Color color)
    : m_arr(sf::TriangleFan, points) {
        float angle = 0;
        float d_angle = 2 * conf::constants::PI / points;
        for (int i = 0; i < points; i++) {
            m_arr.arr[i].position = {center.x + radius * cos(angle), center.y + radius * sin(angle)};
            angle += d_angle;
        }
        for (int i = 0; i < points; i++) {
            m_arr.arr[i].color = color;
        }
    }
    void Circle::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    HollowCircle::HollowCircle(
        sf::Vector2f center, float outerRadius, 
        float strokeWidth, int points, sf::Color color) 
    : m_innerCircle(sf::Points, points), m_outerCircle(sf::Points, points), m_arr(sf::TriangleStrip, points * 2 + 2) {
        float innerRadius = outerRadius - strokeWidth;

        float angle = 0;
        float d_angle = 2 * conf::constants::PI / points;
        for (int i = 0; i < points; i++) {
            m_innerCircle[i].position = {center.x + innerRadius * cos(angle), center.y + innerRadius * sin(angle)};
            m_outerCircle[i].position = {center.x + outerRadius * cos(angle), center.y + outerRadius * sin(angle)};
            m_innerCircle[i].color = color;
            m_outerCircle[i].color = color;
            angle += d_angle;
        }

        for (int i = 0; i < points; i++) {
            m_arr.arr[2 * i] = m_innerCircle[i];
            m_arr.arr[2 * i + 1] = m_outerCircle[i];
        }
        m_arr.arr[points * 2] = m_arr.arr[0];
        m_arr.arr[points * 2 + 1] = m_arr.arr[1];
    }
    void HollowCircle::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    Rect::Rect(sf::Vector2f topLeft, float width, float height, sf::Color color)
    : m_arr(sf::TriangleFan, 4) {
        m_arr.arr[0].position = topLeft;
        m_arr.arr[1].position = {topLeft.x + width, topLeft.y};
        m_arr.arr[2].position = {topLeft.x + width, topLeft.y + height};
        m_arr.arr[3].position = {topLeft.x, topLeft.y + height};

        m_arr.arr[0].color = color;
        m_arr.arr[1].color = color;
        m_arr.arr[2].color = color;
        m_arr.arr[3].color = color;
    }
    Rect::Rect(sf::Vector2f start, sf::Vector2f end, float width, sf::Color color)
    : m_arr(sf::TriangleFan, 4) {
        sf::Vector2f startToEnd = end - start;
        sf::Vector2f normal{-startToEnd.y, startToEnd.x};
        normal = normal / util::getLen(normal) * (width / 2);

        m_arr.arr[0].position = start + normal;
        m_arr.arr[1].position = start - normal;
        m_arr.arr[2].position = end - normal;
        m_arr.arr[3].position = end + normal;

        m_arr.arr[0].color = color;
        m_arr.arr[1].color = color;
        m_arr.arr[2].color = color;
        m_arr.arr[3].color = color;
    }
    void Rect::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    RoundedRect::RoundedRect(
        sf::Vector2f topLeft, float width, float height, float radius, 
        int points_quarterCircle, sf::Color color) 
    : m_arr(sf::TriangleFan, points_quarterCircle * 4) {
        radius = std::min(radius, width / 2.0f);
        radius = std::min(radius, height / 2.0f);
        
        const sf::Vector2f centerTopLeft = {topLeft.x + radius, topLeft.y + radius};
        const sf::Vector2f centerTopRight = {topLeft.x + width - radius, topLeft.y + radius};
        const sf::Vector2f centerBottomRight = {topLeft.x + width - radius, topLeft.y + height - radius};
        const sf::Vector2f centerBottomLeft = {topLeft.x + radius, topLeft.y + height - radius};

        int idx = 0;
        float angle = conf::constants::PI;
        const float d_angle = -(conf::constants::PI / 2) / (points_quarterCircle - 1);

        for (int i = 0; i < points_quarterCircle; i++) {
            m_arr.arr[idx++].position = {centerTopLeft.x + cos(angle) * radius, centerTopLeft.y - sin(angle) * radius};
            angle += d_angle;
        }
        angle = conf::constants::PI / 2;
        for (int i = 0; i < points_quarterCircle; i++) {
            m_arr.arr[idx++].position = {centerTopRight.x + cos(angle) * radius, centerTopRight.y - sin(angle) * radius};
            angle += d_angle;
        }
        angle = 0.0f;
        for (int i = 0; i < points_quarterCircle; i++) {
            m_arr.arr[idx++].position = {centerBottomRight.x + cos(angle) * radius, centerBottomRight.y - sin(angle) * radius};
            angle += d_angle;
        }
        angle = conf::constants::PI * 3 / 2;
        for (int i = 0; i < points_quarterCircle; i++) {
            m_arr.arr[idx++].position = {centerBottomLeft.x + cos(angle) * radius, centerBottomLeft.y - sin(angle) * radius};
            angle += d_angle;
        }
        for (int i = 0; i < idx; i++) {
            m_arr.arr[i].color = color;
        }
    }

    void RoundedRect::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_arr.texture)
            states.texture = m_arr.texture;
        window.draw(m_arr.arr, states);
    }

    RoundedOutlinedRect::RoundedOutlinedRect(
        sf::Vector2f topLeft, float width, float height, float radius, 
        sf::Color color, float outlineWidth, sf::Color outlineColor, int points_quarterCircle) 
    {
        const TextureArray outerOutline = RoundedRect(
            topLeft, width, height, radius, 
            points_quarterCircle, outlineColor).m_arr;

        const TextureArray innerOutline = RoundedRect(
            sf::Vector2f(topLeft.x + outlineWidth, topLeft.y + outlineWidth), 
            std::max(0.0f, width - outlineWidth * 2), 
            std::max(0.0f, height - outlineWidth * 2), 
            std::max(0.0f, radius - outlineWidth), 
            points_quarterCircle,
            outlineColor).m_arr;

        m_innerRect = innerOutline;
        const int POINT_COUNT = m_innerRect.arr.getVertexCount();
        for (int i = 0; i < POINT_COUNT; i++) {
            m_innerRect.arr[i].color = color;
        }
        m_innerRect.arr.setPrimitiveType(sf::TriangleFan);
        
        m_outline.arr.setPrimitiveType(sf::TriangleStrip);
        m_outline.arr.resize(points_quarterCircle * 4 * 2 + 2);

        for (int i = 0; i < points_quarterCircle * 4; i++) {
            m_outline.arr[2 * i].position = innerOutline.arr[i].position;
            m_outline.arr[2 * i + 1].position = outerOutline.arr[i].position;
            m_outline.arr[2 * i].color = innerOutline.arr[i].color;
            m_outline.arr[2 * i + 1].color = outerOutline.arr[i].color;
        }
        m_outline.arr[points_quarterCircle * 4 * 2] = m_outline.arr[0];
        m_outline.arr[points_quarterCircle * 4 * 2 + 1] = m_outline.arr[1];
    }

    void RoundedOutlinedRect::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        if (m_innerRect.texture)
            states.texture = m_innerRect.texture;
        window.draw(m_innerRect.arr, states);

        states.texture = nullptr;
        if (m_outline.texture)
            states.texture = m_outline.texture;
        window.draw(m_outline.arr, states);
    }

    Arrow::Arrow(sf::Vector2f tip, float triangleLength, float triangleWidth, float reactangleLength, float reactangleWidth, float angleRadians, sf::Color color)
    {
        bool pointingRight = (fabs(angleRadians) < 0.01f);
    
        // Create the triangle head at the tip
        Triangle head(tip, triangleLength, triangleWidth, angleRadians, color);
        m_triangle = head.m_arr;
        
        sf::Vector2f shaftTopLeft;
        
        if (pointingRight) {
            shaftTopLeft = sf::Vector2f(
                tip.x - triangleLength - reactangleLength,
                tip.y - reactangleWidth / 2.0f
            );
        } else {
            shaftTopLeft = sf::Vector2f(
                tip.x + triangleLength,
                tip.y - reactangleWidth / 2.0f
            );
        }
        
        Rect shaft(
            shaftTopLeft,
            reactangleLength,
            reactangleWidth,
            color
        );
        m_base = shaft.m_arr;
    }

    void Arrow::draw(sf::RenderTarget& window, sf::RenderStates states) const {
        // Draw the shaft first
        if (m_base.texture)
            states.texture = m_base.texture;
        window.draw(m_base.arr, states);
        
        // Draw the triangle head on top
        states.texture = nullptr;
        if (m_triangle.texture)
            states.texture = m_triangle.texture;
        window.draw(m_triangle.arr, states);
    }

}