#include "UIManager.hpp"
#include "WindowElement.hpp"
#include "Button.hpp"
#include "TextElement.hpp"
#include "../engine/EngineContext.hpp"
#include "../util/util.hpp"
#include "UIAnimation.hpp"
#include "ProgressBar.hpp"
#include "Flex.hpp"
#include "Slider.hpp"

UIManager::UIManager(EngineContext context) : m_engineContext(context) {
    struct Theme {
        ColorHSL green = ColorHSL(0, 191, 160);
        ColorHSL dark_green = ColorHSL(9, 121, 105);
        ColorHSL blue = ColorHSL(0, 128, 255);
        ColorHSL red = ColorHSL(241, 87, 108);
        ColorHSL white = ColorHSL(255, 255, 255);
        ColorHSL lightgrey = ColorHSL(245, 245, 245);
    };
    Theme t;

    m_windowElement = new WindowElement(m_engineContext);
    addElement(m_windowElement, nullptr, nullptr, "window", {});

    m_windowElement->m_context->minSizePasses = 2;
    m_windowElement->m_context->minTransformPasses = 2;
}

void UIManager::addElement(
        UIElement* elem, UIElement* parent, UIElement* anchor, 
        std::string elementId, std::vector<std::string> elementClasses) {

    if (m_idToElement.find(elementId) != m_idToElement.end()) {
        std::cerr << "Duplicate ID: " << elementId << std::endl;
        return;
    }

    m_idToElement[elementId] = elem;
    elem->m_anchor = anchor;
    elem->m_parent = parent;
    elem->m_context->id = elementId;

    if (parent != nullptr) {
        parent->m_children.push_back(elem);
        elem->m_context->z_index = parent->m_context->z_index + 1;
    }

    for (int i = 0; i < elementClasses.size(); i++) {
        applyClass(elem, elementClasses[i]);
    }
}

void UIManager::draw() {
    std::vector<UIElement*> drawables;
    for (UIElement* c : m_windowElement->m_children) {
        c->propogateCall([&drawables](UIElement* e) {
            if (e->m_context->m_current.state == UIElementState::HIDDEN || e->m_context->calculatedOpacity < 0.001f) {
                return false;
            }
            drawables.push_back(e);
            return !(e->m_context->overflow[0] == UIOverflowMode::HIDDEN ||
                e->m_context->overflow[1] == UIOverflowMode::HIDDEN ||
                e->m_context->overflow[0] == UIOverflowMode::SCROLL || 
                e->m_context->overflow[1] == UIOverflowMode::SCROLL || 
                e->m_context->dontDrawChildren);
        });
    }
    std::stable_sort(drawables.begin(), drawables.end(), [](UIElement* l, UIElement* r) {
        return l->m_context->z_index < r->m_context->z_index;
    });
    sf::IntRect window_viewport = m_engineContext.m_window->getViewport(m_engineContext.m_window->getView());
    for (int i = 0; i < drawables.size(); i++) {
        drawables[i]->draw(m_engineContext.m_window, window_viewport);
    }

    for (int i = 0; i < m_endOfFrameDraws.size(); i++) {
        m_engineContext.m_window->draw(m_endOfFrameDraws[i]);
    }
    m_endOfFrameDraws.clear();
}

void UIManager::update() {
    m_mouseContext.m_current.pos = sf::Mouse::getPosition(*m_engineContext.m_window);

    std::vector<UIElement*> updateables;
    m_windowElement->propogateCall([&updateables](UIElement* e) {
        if (e->m_context->dontUpdateYourself || e->m_context->m_current.state == UIElementState::HIDDEN || e->m_context->m_current.state == UIElementState::DISABLED) {
            e->m_context->mousePressedHere = false;
            return false;
        }
        updateables.push_back(e);
        return !e->m_context->dontUpdateChildren;
    });
    std::reverse(updateables.begin(), updateables.end());
    std::stable_sort(updateables.begin(), updateables.end(), [](UIElement* l, UIElement* r) {
        return l->m_context->z_index > r->m_context->z_index;
    });
    for (int i = 0; i < updateables.size(); i++) {
        updateables[i]->update(m_mouseContext, m_keyboardContext);
    }
    m_windowElement->propogateCall([](UIElement* e){
        e->checkChangedStates();
        return true;
    });

    m_windowElement->checkDirtyBounds();
    m_windowElement->checkDirtyTransforms();
    m_windowElement->propogateCall([](UIElement* e) {
        if (e->m_context->dirtyBounds || e->m_context->minSizePasses > 0) {
            if (e->m_context->minSizePasses > 0) {
                e->m_context->minSizePasses--;
            }
            e->computeSize();
            return false;
        }
        return true;
    });
    m_windowElement->propogateCall([](UIElement* e) {
        if (e->m_context->dirtyTransform || e->m_context->minTransformPasses > 0) {
            if (e->m_context->minTransformPasses > 0) {
                e->m_context->minTransformPasses--;
            }
            e->computeTransforms();
            return false;
        }
        return true;
    });
}

void UIManager::handleEvent(sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        m_keyboardContext.press(e.key.code);
    }
    else if (e.type == sf::Event::KeyReleased) {
        m_keyboardContext.release(e.key.code);
    }

    if (e.type == sf::Event::MouseButtonPressed) {
        m_mouseContext.press(e.mouseButton.button);
    }
    else if (e.type == sf::Event::MouseButtonReleased) {
        m_mouseContext.release(e.mouseButton.button);
    }
    else if (e.type == sf::Event::MouseWheelScrolled) {
        m_mouseContext.m_current.mouseWheelScrolled = true;
        m_mouseContext.m_current.scrollDelta = e.mouseWheelScroll.delta;
    }
}

UIElement* UIManager::getElementById(std::string id) {
    if (m_idToElement.find(id) != m_idToElement.end())
        return m_idToElement[id];
    return nullptr;
}

void UIManager::registerClass(std::string newClass, std::function<void(UIElement*)> callback) {
    m_classToCallback[newClass] = callback;
}

void UIManager::startFrame() {
    m_windowElement->propogateCall([](UIElement* e) {
        e->m_context->startFrame();
        return true;
    });

    for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
        m_keyboardContext.m_current.isTriggeredThisFrame[i] = false;
        m_keyboardContext.m_current.isConsumedThisFrame[i] = false;
    }

    m_mouseContext.m_current.mouseWheelScrolled = false;
    for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
        m_mouseContext.m_current.isConsumedThisFrame[i] = false;
    }
}
void UIManager::endFrame() {
    m_windowElement->propogateCall([](UIElement* e) {
        e->m_context->endFrame();
        return true;
    });

    m_keyboardContext.m_prev = m_keyboardContext.m_current;
    m_mouseContext.m_prev = m_mouseContext.m_current;
}

void UIManager::applyClass(UIElement* element, std::string element_class) {
    if (m_classToCallback.find(element_class) != m_classToCallback.end()) {
        m_classToCallback[element_class](element);
    }
    else {
        std::cout << "Unknown class when trying to initialize id [" << element->m_context->id << "]: " << element_class << '\n';
    }
}