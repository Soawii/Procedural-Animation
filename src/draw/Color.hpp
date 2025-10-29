#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class ColorHSL {
public:
    ColorHSL() = default;
    explicit ColorHSL(int r, int g, int b, int a = 255);
    explicit ColorHSL(float h, float s, float l, float a = 1.0f);

    float h = 0.0f; // 0-360
    float s = 0.0f; // 0-1
    float l = 0.0f; // 0-1
    float a = 1.0f; // 0-1

    ColorHSL operator+(const ColorHSL& other) const;
    ColorHSL operator-(const ColorHSL& other) const;
    ColorHSL operator*(float scalar) const;
    bool operator!=(const ColorHSL& other) const;
    bool operator==(const ColorHSL& other) const;
};

ColorHSL rgbToHsl(const sf::Color& c);
sf::Color hslToRgb(const ColorHSL& hsl);

ColorHSL lerpColor(const ColorHSL& c1, const ColorHSL& c2, float t);
sf::Color lerpColor(const sf::Color& c1, const sf::Color& c2, float t);