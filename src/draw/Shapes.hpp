#pragma once
#include <SFML/Graphics.hpp>
#include "../conf.hpp"

namespace shapes {
    class TextureArray {
    public:
        TextureArray() {}
        TextureArray(const sf::PrimitiveType type, const size_t points) : arr(type, points) {}
        sf::VertexArray arr;
        sf::Texture* texture = nullptr;
    };

    void bindTexture(TextureArray& arr, sf::Texture* texture);
    void bindTexture(TextureArray& arr, sf::Texture* texture, const sf::Vector2f pos, const sf::Vector2f size);

    class Shape : public sf::Drawable {
    };

    class Line : public Shape {
    public:
        Line(sf::Vector2f start, sf::Vector2f end, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_arr;
    };

    class Triangle : public Shape {
    public:
        Triangle(sf::Vector2f tip, float length, float width, float pointingAngle, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_arr;
    };

    class Circle : public Shape {
    public:
        Circle(sf::Vector2f center, float radius, int points = 50, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_arr;
    };

    class HollowCircle : public Shape {
    public:
        HollowCircle(sf::Vector2f center, float outerRadius, float strokeWidth, int points = 50, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        sf::VertexArray m_innerCircle, m_outerCircle;
        TextureArray m_arr;
    };

    class Rect : public Shape {
    public:
        Rect(sf::Vector2f topLeft, float width, float height, sf::Color color = sf::Color(255,255,255));
        Rect(sf::Vector2f start, sf::Vector2f end, float width, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_arr;
    };

    class RoundedRect : public Shape {
    public:
        RoundedRect(sf::Vector2f topLeft, float width, float height, float radius, int points_quarterCircle = 15, sf::Color color = sf::Color(255,255,255));
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_arr;
    };

    class RoundedOutlinedRect : public Shape {
    public:
        RoundedOutlinedRect(sf::Vector2f topLeft, float width, float height, float radius, sf::Color color = sf::Color(255,255,255), float outlineWidth = 0.0f, sf::Color outlineColor = sf::Color(230,230,230), int points_quarterCircle = 10);
        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        TextureArray m_innerRect, m_outline;
    };

    class Arrow : public Shape {
    public:
        Arrow(sf::Vector2f tip, float triangleLength, float triangleWidth, float reactangleLength, float reactangleWidth, float angleRadians = 0.0f, sf::Color color = sf::Color::White);

        void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
        
        TextureArray m_triangle;
        TextureArray m_base;
    };

}