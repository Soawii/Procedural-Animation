#pragma once
#include "UIElement.hpp"
#include <string>
#include <unordered_map>
#include "../engine/EngineContext.hpp"
#include "WindowElement.hpp"
#include <vector>
#include "../draw/Shapes.hpp"

class UIManager {
public:
    UIManager(EngineContext context);

    void addElement(UIElement* elem, UIElement* parent, UIElement* anchor, 
        std::string elementId, std::vector<std::string> elementClasses);
    void draw();
    void update();
    void handleEvent(sf::Event& e);

    UIElement* getElementById(std::string id);
    void registerClass(std::string newClass, std::function<void(UIElement*)> callback);
    void applyClass(UIElement* element, std::string element_class);

    void startFrame();
    void endFrame();

    WindowElement* m_windowElement;
    UIElement* m_globalModal = nullptr;

    KeyboardContext m_keyboardContext;
    MouseContext m_mouseContext;
private:
    EngineContext m_engineContext;
    std::unordered_map<std::string, std::function<void(UIElement*)>> m_classToCallback;
    std::unordered_map<std::string, UIElement*> m_idToElement;
    std::vector<shapes::Shape> m_endOfFrameDraws;
};