#include "Button.hpp"
#include "../draw/Shapes.hpp"
#include <iostream>

Button::Button()
: UIElement() {
    m_context->onClick.push_back([this]() {this->click();});
}

void Button::click() {
    if (m_context->pressable) {
        bool prev_pressed = m_context->pressed;
        m_context->pressed = !m_context->pressed;
        if (m_radio) {
            if (m_context->pressed) {
                for (Button* b : *m_radioGroup) {
                    if (this == b) {
                        continue;
                    }
                    if (b->m_context->pressed) {
                        b->click();
                    }
                }
            }
            else {
                int pressed_count = 0;
                for (Button* b : *m_radioGroup) {
                    if (this == b) {
                        continue;
                    }
                    if (b->m_context->pressed) {
                        pressed_count++;
                    }
                }
                if (pressed_count == 0) {
                    m_context->pressed = !m_context->pressed;
                }
            }
        }
        if (m_context->pressed != prev_pressed) {
            m_context->triggerEvents(m_context->onPressChange);
        }
    }
}