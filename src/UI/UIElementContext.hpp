#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "../util/TimingVariable.hpp"
#include <string>
#include <vector>
#include "../draw/Color.hpp"
#include "UIElementState.hpp"
#include "Bounds.hpp"
#include <array>
#include "../draw/Shapes.hpp"

struct UIElementFrameContext {
    UIElementState state = UIElementState::NORMAL;
    bool isMouseIn = false;
};

class UIElementContext {
public:
    UIElementContext();

    UIElementFrameContext m_prev, m_current;

    std::string id;

    int z_index = 0;
    int contentSizeComputed = 0;
    int minSizePasses = 0, minTransformPasses = 0;

    bool fitContentFixed = false, allContentFixed = false;
    bool copyTransformToChildren = false;
    bool dontDrawChildren = false;
    bool dontUpdateChildren = false, dontUpdateYourself = false;

    bool pushed = false;
    bool pressable = false, pressed = false;
    bool focusable = false, focused = false;

    bool dirtyBounds = false; 
    bool dirtyTransform = false;

    Bounds childrenBounds, anchorBounds, mouseBounds;

    sf::Transform calculatedTransform;
    float calculatedOpacity;

    UISizeMode sizeMode[2] = {UISizeMode::FIXED, UISizeMode::FIXED};

    // scroll
    UIOverflowMode overflow[2] = {UIOverflowMode::VISIBLE, UIOverflowMode::VISIBLE};
    float scrollbarWidth = 10.0f;
    float scrollbarBorderRadius = 10.0f;
    float scrollWheelDistance = 80.0f;
    TimingVariable<float> scrollPosX, scrollPosY;
    float scrollCalulated[2] = {0.0f, 0.0f};
    bool mousePressedOnScroll[2] = {false, false};

    bool mousePressedHere = false;
    bool propogateMouse[sf::Mouse::ButtonCount];
    bool propogateKeyboard[sf::Keyboard::KeyCount];
    std::vector<std::pair<sf::Keyboard::Key, std::function<void()>>> onPressKeys;
    std::vector<std::pair<sf::Keyboard::Key, std::function<void()>>> onTriggerKeys;

    std::vector<std::function<void()>> onClick;
    std::vector<std::function<void()>> onPress;
    std::vector<std::function<void()>> onPressChange;
    std::vector<std::function<void()>> onMouseEnter;
    std::vector<std::function<void()>> onMouseLeave;
    std::vector<std::function<void()>> whileActive;
    std::vector<std::function<void()>> onStateChange[size_t(UIElementState::STATE_AMOUNT)];

    bool didMouseMoveIn();
    bool didMouseMoveOut();

    bool wasStateChanged();
    void triggerEvents(const std::vector<std::function<void()>>& events);
    void changedState();

    void startFrame();
    void endFrame();
};