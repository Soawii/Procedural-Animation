#include "TextElement.hpp"

TextElement::TextElement(const sf::String& text, const sf::Font& font, unsigned int charSize, unsigned int thickness)
{

    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(charSize);
    if (thickness != 0)
        m_text.setOutlineThickness(thickness);
    m_context->sizeMode[0] = UISizeMode::FIT_CONTENT;
    m_context->sizeMode[1] = UISizeMode::FIT_CONTENT;

    for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
        m_context->propogateMouse[i] = true;
    }
    for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
        m_context->propogateKeyboard[i] = true;
    }
}

sf::Vector2f TextElement::getLocalBounds() {
    sf::Vector2f bounds = m_text.getLocalBounds().getSize();
    if (m_context->sizeMode[0] == UISizeMode::FIXED || m_context->sizeMode[1] == UISizeMode::FIXED) {
        const Bounds& anchorRect = (m_parent == m_anchor 
            ? m_anchor->m_context->childrenBounds
            : m_anchor->m_context->anchorBounds);
        const sf::Vector2f absoluteSize = m_vars.size.absolute.get();
        const sf::Vector2f relativeSize = m_vars.size.relative.get();
        if (m_context->sizeMode[0] == UISizeMode::FIXED) {
            bounds.x = relativeSize.x * anchorRect.m_width + absoluteSize.x;
        }
        if (m_context->sizeMode[1] == UISizeMode::FIXED) {
            bounds.y = relativeSize.y * anchorRect.m_height + absoluteSize.y;
        }
    }
    if (m_fixedHeight.x != 0.0f) {
        bounds.y = m_fixedHeight.x;
    }
    return bounds;
}

void TextElement::computeSize() {
    computeBounds();

    if (m_fitParent) {
        const Bounds& anchorRect = m_parent->m_context->childrenBounds;

        sf::Text temp = m_text;
        const sf::String& str = m_text.getString();
        sf::String wrapped;
        const float maxWidth = anchorRect.m_width;
        int start = 0;
        for (size_t i = 0; i < str.getSize(); ++i) {
            sf::Uint32 c = str[i];
            if (c == ' ' || i == str.getSize() - 1) {
                sf::String word = str.substring(start, i - start + (i == str.getSize() - 1 ? 1 : 0));
                temp.setString(wrapped + word);
                float newWidth = temp.getLocalBounds().width;
                if (newWidth > maxWidth) {
                    word[0] = '\n';
                }
                start = i;
                wrapped += word;
                word.clear();
            }
        }
        m_text.setString(wrapped);
    }
}

void TextElement::setString(const sf::String& str) {
    m_text.setString(str);
    computeSize();
}

void TextElement::setFixedHeight(std::string possibleChars) {
    sf::Text text;
    text.setCharacterSize(m_text.getCharacterSize());
    text.setFont(*m_text.getFont());
    text.setString(possibleChars);
    const sf::FloatRect bounds = text.getLocalBounds();
    m_fixedHeight.x = bounds.getSize().y;
    m_fixedHeight.y = bounds.getPosition().y;
}

void TextElement::draw(sf::RenderWindow *window, sf::IntRect& viewport) {
    const float top = m_context->mouseBounds.m_pos.y;
    const float left = m_context->mouseBounds.m_pos.x;
    const float bottom = m_context->mouseBounds.m_pos.y + m_context->anchorBounds.m_height;
    const float right = m_context->mouseBounds.m_pos.x + m_context->anchorBounds.m_width;
    if (bottom < viewport.top || 
            right < viewport.left ||
            left > viewport.left + viewport.width || 
            top > viewport.top + viewport.height) {
        return;
    }

    const Bounds& bounds = m_context->mouseBounds;
    if (m_changeOrigin) {
        sf::Vector2f pos = m_text.getLocalBounds().getPosition();
        if (m_fixedHeight.x != 0.0f) {
            pos.y = m_fixedHeight.y;
        }   
        m_text.setOrigin(int(pos.x), int(pos.y));
    }

    sf::Vector2f pos = m_context->mouseBounds.m_pos;
    pos.x = int(pos.x);
    pos.y = int(pos.y);

    const ColorHSL colorHSL = m_vars.color.get();
    sf::Color color = hslToRgb(colorHSL);
    color.a *= m_context->calculatedOpacity;

    m_text.setPosition(pos);
    m_text.setFillColor(color);
    window->draw(m_text, m_context->calculatedTransform);
}