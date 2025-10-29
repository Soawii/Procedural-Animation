#pragma once
#include "UIElement.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class TextElement : public UIElement {
public:
    TextElement(const sf::String& text, const sf::Font& font, unsigned int charSize = 24, unsigned int thickness = 0);

    sf::Vector2f getLocalBounds() override;
    void computeSize() override;

    void setString(const sf::String& str);
    void setFixedHeight(std::string possibleChars);
    void setFillColor(const sf::Color& color);
    void draw(sf::RenderWindow* window, sf::IntRect& viewport) override;

    sf::Text m_text;
    bool m_fitParent = false;
    bool m_changeOrigin = true;
    sf::Vector2f m_fixedHeight = {0.0f, 0.0f};
};