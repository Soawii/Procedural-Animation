#pragma once
#include <SFML/Graphics.hpp>
#include "MouseContext.hpp"
#include "KeyboardContext.hpp"
#include "UIElementContext.hpp"
#include <string>
#include "../draw/Shapes.hpp"
#include "../draw/Color.hpp"
#include <iostream>
#include <vector>
#include "StateVariable.hpp"
#include "Bounds.hpp"
#include "UIAnimation.hpp"

class UIElement {
public:
    UIElement();

    virtual bool checkDirtyBounds();
    virtual void checkDirtyTransforms();

    virtual sf::Vector2f getLocalBounds();
    virtual sf::FloatRect getGlobalBounds();
    virtual void computeBounds();
    virtual void computeSize();
    virtual void computeTransforms();

    virtual void propogateCall(const std::function<bool(UIElement*)>& func);

    virtual void draw(sf::RenderWindow* window, sf::IntRect& viewport);
    virtual void update(MouseContext& mouseContext, KeyboardContext& keyboardContext);

    void setState(UIElementState state);
    virtual void checkChangedStates();

    void addChild(UIElement* child, UIElement* anchor = nullptr);
    UIElement* getChildById(std::string id);

    UIElement* m_parent = nullptr;
    UIElement* m_anchor = nullptr;
    std::vector<UIElement*> m_children;
    std::vector<IUIAnimation*> m_animations;
    UIElementContext* m_context = new UIElementContext();
    StateVariables m_vars;
};
