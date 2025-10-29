#pragma once
#include "UIElement.hpp"
#include <functional>
#include "UIElementContext.hpp"
#include <vector>

class Button : public UIElement {
public:
    Button();

    void click();

    bool m_radio = false;
    std::vector<Button*>* m_radioGroup;
};