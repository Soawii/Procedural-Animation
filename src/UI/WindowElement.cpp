#include "WindowElement.hpp"
#include "../sim/Snake.hpp"
#include "../sim/Lizard.hpp"
#include <iostream>
#include <algorithm>

WindowElement::WindowElement(EngineContext context) : UIElement(), m_engineContext(context) {}

sf::FloatRect WindowElement::getGlobalBounds() {
    sf::Vector2u windowSize = m_engineContext.m_window->getSize();
    return sf::FloatRect(0.0f, 0.0f, windowSize.x, windowSize.y);
}

void WindowElement::computeSize() {
    sf::Vector2u windowSize = m_engineContext.m_window->getSize();
    m_context->mouseBounds = Bounds({0.0f, 0.0f}, windowSize.x, windowSize.y);
    m_context->anchorBounds = Bounds({0.0f, 0.0f}, windowSize.x, windowSize.y);
    m_context->childrenBounds = Bounds({0.0f, 0.0f}, windowSize.x, windowSize.y);

    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->computeSize();
    }
}
bool WindowElement::checkDirtyBounds() {
   sf::Vector2u windowSize = m_engineContext.m_window->getSize();
    if (fabs(m_context->mouseBounds.m_width - windowSize.x) > 0.01f || 
        fabs(m_context->mouseBounds.m_height - windowSize.y) > 0.01f) 
    {
        m_context->dirtyBounds =  true;
        return true;
    }

    bool childDirty = false;
    for (UIElement* child : m_children) {
        childDirty = childDirty || child->checkDirtyBounds();
    }
    return childDirty;
}

void WindowElement::draw(sf::RenderWindow* window, sf::IntRect& viewport) {
    
}

void WindowElement::update(MouseContext& mouseContext, KeyboardContext& keyboardContext) {
    if (mouseContext.isPressed(sf::Mouse::Left) && !mouseContext.isConsumed(sf::Mouse::Left)) {
        m_engineContext.m_sim->m_entities[m_engineContext.m_sim->m_chosen_entity]->m_targetPoint = sf::Vector2f(mouseContext.m_current.pos);
    }
}
