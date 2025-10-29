#include "UIElement.hpp"
#include "ShapePool.hpp"

UIElement::UIElement() : m_vars(m_context) {
}

sf::Vector2f UIElement::getLocalBounds() {
    const Bounds& anchorRect = m_parent->m_context->childrenBounds;
    const sf::Vector2f absoluteSize = m_vars.size.absolute.get();
    const sf::Vector2f relativeSize = m_vars.size.relative.get();
    return sf::Vector2f(
        relativeSize.x * anchorRect.m_width + absoluteSize.x,
        relativeSize.y * anchorRect.m_height + absoluteSize.y
    );
}

sf::FloatRect UIElement::getGlobalBounds() {
    const Bounds& anchorRect = (m_parent == m_anchor 
        ? m_anchor->m_context->childrenBounds
        : m_anchor->m_context->anchorBounds);

    const sf::Vector2f localBounds = getLocalBounds();
    const sf::Vector2f absolutePos = m_vars.pos.absolute.get();
    const sf::Vector2f relativePos = m_vars.pos.relative.get();
    const sf::Vector2f absoluteOrigin = m_vars.origin.absolute.get();
    const sf::Vector2f relativeOrigin = m_vars.origin.relative.get();

    const sf::Vector2f pos = {
        relativePos.x * anchorRect.m_width 
        + absolutePos.x + anchorRect.m_pos.x
        - absoluteOrigin.x - relativeOrigin.x * localBounds.x
        - m_parent->m_context->scrollPosX.get(),

        relativePos.y * anchorRect.m_height 
        + absolutePos.y + anchorRect.m_pos.y
        - absoluteOrigin.y - relativeOrigin.y * localBounds.y
        - m_parent->m_context->scrollPosY.get()
    };

    return sf::FloatRect(pos.x, pos.y, localBounds.x, localBounds.y);
}

void UIElement::computeBounds() {
    const float borderWidth = m_vars.borderWidth.get();
    const float padding = m_vars.padding.get();
    const sf::FloatRect bounds = getGlobalBounds();

    const float x_scroll_height = m_context->overflow[0] == UIOverflowMode::SCROLL ? m_context->scrollbarWidth : 0.0f;
    const float y_scroll_width = m_context->overflow[1] == UIOverflowMode::SCROLL ? m_context->scrollbarWidth : 0.0f;

    m_context->mouseBounds = Bounds(
        {bounds.left, bounds.top}, 
        bounds.width - y_scroll_width, 
        bounds.height - x_scroll_height);
    m_context->mouseBounds.m_radius = m_vars.borderRadius.get();

    m_context->childrenBounds = Bounds(
        { bounds.left + borderWidth + padding,
        bounds.top + borderWidth + padding}, 
        bounds.width - borderWidth * 2 - padding * 2 - y_scroll_width,
        bounds.height - borderWidth * 2 - padding * 2 - x_scroll_height
    );

    m_context->anchorBounds = Bounds(
        {bounds.left, bounds.top},
        bounds.width, bounds.height);
    m_context->anchorBounds.m_radius = m_context->mouseBounds.m_radius;
}

void UIElement::computeSize() {
    //if (m_context->m_current.state == UIElementState::DISABLED && m_context->calculatedOpacity < 0.001f) {
    //    return;
    //}

    if (m_context->allContentFixed && m_context->contentSizeComputed > 2) {
        computeBounds();
        return;
    }
    if (m_context->fitContentFixed && m_context->contentSizeComputed > 0) {
        propogateCall([](UIElement* e) {
            e->computeBounds();
            return true;
        });
        return;
    }

    if (m_context->sizeMode[0] == UISizeMode::FIXED || m_context->sizeMode[1] == UISizeMode::FIXED) {
        computeBounds();
    }
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->computeSize();
    }

    if (m_context->sizeMode[0] == UISizeMode::FIT_CONTENT || 
            m_context->sizeMode[1] == UISizeMode::FIT_CONTENT || 
            m_context->overflow[0] == UIOverflowMode::SCROLL || 
            m_context->overflow[1] == UIOverflowMode::SCROLL) {
        const float borderWidth = m_vars.borderWidth.get();
        const float padding = m_vars.padding.get();
        const float x_scroll_height = m_context->overflow[0] == UIOverflowMode::SCROLL ? m_context->scrollbarWidth : 0.0f;
        const float y_scroll_width = m_context->overflow[1] == UIOverflowMode::SCROLL ? m_context->scrollbarWidth : 0.0f;
        const sf::Vector2f size = m_vars.size.absolute.m_var.m_var.getActual();
        sf::Vector2f newSize = size;
        
        if (m_context->sizeMode[0] == UISizeMode::FIT_CONTENT || m_context->overflow[0] == UIOverflowMode::SCROLL) {
            if (m_children.size() == 0) {
                newSize.x = borderWidth * 2 + padding * 2;
            }
            else {
                float x_min = 10000000.0f;
                float x_max = -10000000.0f;

                for (int i = 0; i < m_children.size(); i++) {
                    UIElement* c = m_children[i];
                    if (c->m_context->m_current.state == UIElementState::DISABLED)
                        continue;

                    sf::FloatRect bounds = c->getGlobalBounds();
                    x_min = std::min(x_min, bounds.left);
                    x_max = std::max(x_max, bounds.left + bounds.width);
                }
                newSize.x = (x_max - x_min) + borderWidth * 2 + padding * 2 + y_scroll_width;
            }
            m_context->scrollCalulated[0] = newSize.x;
        }
        if (m_context->sizeMode[1] == UISizeMode::FIT_CONTENT || m_context->overflow[1] == UIOverflowMode::SCROLL) {
            if (m_children.size() == 0) {
                newSize.y = borderWidth * 2 + padding * 2;
            }
            else {
                float y_min = 10000000.0f;
                float y_max = -10000000.0f;

                for (int i = 0; i < m_children.size(); i++) {
                    UIElement* c = m_children[i];
                    if (c->m_context->m_current.state == UIElementState::DISABLED)
                        continue;

                    sf::FloatRect bounds = c->getGlobalBounds();
                    y_min = std::min(y_min, bounds.top);
                    y_max = std::max(y_max, bounds.top + bounds.height);
                }
                newSize.y = (y_max - y_min) + borderWidth * 2 + padding * 2 + x_scroll_height;
            }
            m_context->scrollCalulated[1] = newSize.y;
        }

        if ((m_context->sizeMode[0] == UISizeMode::FIT_CONTENT || m_context->sizeMode[1] == UISizeMode::FIT_CONTENT) && 
                (fabs(newSize.x - size.x) > 0.001 || fabs(newSize.y - size.y) > 0.001)) {
            m_vars.size.absolute.setAllSmoothly(newSize);
        }
        computeBounds();
    }
    if (m_context->contentSizeComputed < 5)
        m_context->contentSizeComputed++;
}

void UIElement::computeTransforms() {
    // Compute the transform and opacity of the element and its children.
    // It's combined with the calculated transform and opacity of the parent [parentTransform, parentOpacity].

    // This is done after the computeSize() function because translate.relative depends on the size of an element.
    // For example, if we set relative translate X to -0.5f, the element will move 50% of its size to the left.
    // This is done so we can easily center elements without using element's origin or flex elements.

    //if (m_context->m_current.state == UIElementState::DISABLED && fabs(m_context->calculatedOpacity) < 0.001) {
    //    return;
    //}

    const Bounds& bounds = m_context->mouseBounds;

    const float scale = m_vars.scale.get();
    const float rotate = m_vars.rotate.get();
    const sf::Vector2f absTranslate = m_vars.translate.absolute.get();
    const sf::Vector2f relTranslate = m_vars.translate.relative.get();
    const sf::Vector2f absTransformOrigin = m_vars.transformOrigin.absolute.get();
    const sf::Vector2f relTransformOrigin = m_vars.transformOrigin.relative.get();

    const sf::Vector2f translate = {
        absTranslate.x + relTranslate.x * bounds.m_width,
        absTranslate.y + relTranslate.y * bounds.m_height
    };
    const sf::Vector2f transformOrigin = {
        bounds.m_pos.x + absTransformOrigin.x + relTransformOrigin.x * bounds.m_width,
        bounds.m_pos.y + absTransformOrigin.y + relTransformOrigin.y * bounds.m_height
    };

    sf::Transform t;
    t.scale(sf::Vector2f(scale, scale), transformOrigin);
    t.rotate(rotate, transformOrigin);
    t.translate(translate);
    const float opacity = m_vars.opacity.get();

    const sf::Transform totalTransform = (m_parent ? m_parent->m_context->calculatedTransform : sf::Transform()) * t;
    const float totalOpacity = (m_parent ? m_parent->m_context->calculatedOpacity : 1.0f) * opacity;

    if (m_context->copyTransformToChildren) {
        propogateCall([totalTransform, totalOpacity](UIElement* e) {
            e->m_context->calculatedTransform = totalTransform;
            e->m_context->mouseBounds.m_transform = totalTransform;
            e->m_context->anchorBounds.m_transform = totalTransform;
            e->m_context->childrenBounds.m_transform = totalTransform;
            e->m_context->calculatedOpacity = totalOpacity;
            return true;
        });
    }
    else {
        m_context->calculatedTransform = totalTransform;
        m_context->mouseBounds.m_transform = totalTransform;
        m_context->childrenBounds.m_transform = totalTransform;
        m_context->anchorBounds.m_transform = totalTransform;
        m_context->calculatedOpacity = totalOpacity;
        for (UIElement* child : m_children) {
            child->computeTransforms();
        }
    }
}

void UIElement::propogateCall(const std::function<bool(UIElement*)>& func) {
    const bool continue_propogation = func(this);
    if (continue_propogation) {
        for (int i = 0; i < m_children.size(); i++) {
            m_children[i]->propogateCall(func);
        }
    }
}

void UIElement::draw(sf::RenderWindow* window, sf::IntRect& viewport) {
    // Return if an element is outside the viewport
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

    // Get all the values of state variables, draw the main rounded outline rectangle shape.
    const float borderWidth = m_vars.borderWidth.get();
    const float borderRadius = m_vars.borderRadius.get();
    const ColorHSL colorHSL = m_vars.color.get();
    const ColorHSL borderColorHSL = m_vars.borderColor.get();
    sf::Color color = hslToRgb(colorHSL);
    sf::Color borderColor = hslToRgb(borderColorHSL);
    color.a *= m_context->calculatedOpacity;
    borderColor.a *= m_context->calculatedOpacity;

    sf::Transform calculatedTransform = m_context->calculatedTransform;
    calculatedTransform.translate(m_context->mouseBounds.m_pos);
    if (color.a > 0.005f || (borderColor.a > 0.005f && borderWidth > 0.5f)) {
        const RoundedOutlinedRectParams p(m_context->anchorBounds.m_width, m_context->anchorBounds.m_height, borderWidth, borderRadius);
        const shapes::RoundedOutlinedRect *shape = ShapePool::getAddRoundedOutlinedRect(p, color, borderColor);
        
        window->draw(*shape, calculatedTransform);
    }
}

void UIElement::update(MouseContext& mouseContext, KeyboardContext& keyboardContext) {
    if (m_context->m_current.state == UIElementState::HIDDEN || m_context->m_current.state == UIElementState::DISABLED) {
        m_context->mousePressedHere = false;
        return;
    }

    bool isMouseIn = m_context->mouseBounds.doesPointIntersect(sf::Vector2f(mouseContext.m_current.pos));
    if (isMouseIn && m_parent != nullptr && 
            (m_parent->m_context->overflow[0] == UIOverflowMode::HIDDEN || m_parent->m_context->overflow[1] == UIOverflowMode::HIDDEN ||
            m_parent->m_context->overflow[0] == UIOverflowMode::SCROLL || m_parent->m_context->overflow[1] == UIOverflowMode::SCROLL)) {
        isMouseIn = isMouseIn && m_parent->m_context->mouseBounds.doesPointIntersect(sf::Vector2f(mouseContext.m_current.pos));
    }
    m_context->m_current.isMouseIn = isMouseIn;

    if (isMouseIn && !m_context->m_prev.isMouseIn) {
        m_context->triggerEvents(m_context->onMouseEnter);
    }
    if (!isMouseIn && m_context->m_prev.isMouseIn) {
        m_context->triggerEvents(m_context->onMouseLeave);
    }

    if (isMouseIn && mouseContext.isPressedThisFrame(sf::Mouse::Left) && !mouseContext.isConsumedThisFrame(sf::Mouse::Left)) {
        m_context->mousePressedHere = true;
        m_context->triggerEvents(m_context->onPress);
        if (!m_context->propogateMouse[sf::Mouse::Left]) {
            mouseContext.consume(sf::Mouse::Left);
        }
    }
    if (mouseContext.isReleasedThisFrame(sf::Mouse::Left)) {
        if (m_context->mousePressedHere) {
            if (isMouseIn) {
                m_context->triggerEvents(m_context->onClick);
            }
            if (!m_context->propogateMouse[sf::Mouse::Left])
                mouseContext.m_current.isConsumed[sf::Mouse::Left] = false;
        }
        m_context->mousePressedHere = false;
    }

    if (m_context->focusable && m_context->focused && !isMouseIn && mouseContext.isPressedThisFrame(sf::Mouse::Left)) {
        m_context->focused = false;
    }
    if (m_context->focusable && m_context->focused) {
        for (int i = 0; i < m_context->onPressKeys.size(); i++) {
            const sf::Keyboard::Key key = m_context->onPressKeys[i].first;
            if (keyboardContext.isPressedThisFrame(key) && !keyboardContext.isConsumedThisFrame(key)) {
                m_context->onPressKeys[i].second();
                if (!m_context->propogateKeyboard[key])
                    keyboardContext.consume(key);
            }
        }
        for (int i = 0; i < m_context->onPressKeys.size(); i++) {
            const sf::Keyboard::Key key = m_context->onPressKeys[i].first;
            if (keyboardContext.isReleasedThisFrame(key) && keyboardContext.isConsumed(key)) {
                keyboardContext.m_current.isConsumed[key] = false;
                keyboardContext.m_current.isConsumedThisFrame[key] = false;
            }
        }
        for (int i = 0; i < m_context->onTriggerKeys.size(); i++) {
            const sf::Keyboard::Key key = m_context->onTriggerKeys[i].first;
            if (keyboardContext.isTriggeredThisFrame(key) && !keyboardContext.isConsumedThisFrame(key)) {
                m_context->onTriggerKeys[i].second();
                if (!m_context->propogateKeyboard[key])
                    keyboardContext.consume(key);
            }
        }
        for (int i = 0; i < m_context->onTriggerKeys.size(); i++) {
            const sf::Keyboard::Key key = m_context->onTriggerKeys[i].first;
            if (keyboardContext.isReleasedThisFrame(key) && keyboardContext.isConsumed(key)) {
                keyboardContext.m_current.isConsumed[key] = false;
                keyboardContext.m_current.isConsumedThisFrame[key] = false;
            }
        }
    }

    if (m_context->overflow[0] == UIOverflowMode::SCROLL || m_context->overflow[1] == UIOverflowMode::SCROLL) {
        const bool isMouseInScroll = !isMouseIn && m_context->anchorBounds.doesPointIntersect(sf::Vector2f(mouseContext.m_current.pos));
        const sf::Vector2f point_inversed = m_context->anchorBounds.m_transform.getInverse().transformPoint(sf::Vector2f(mouseContext.m_current.pos));
    
        UIElementState state = UIElementState::NORMAL;

        if (isMouseInScroll)
            state = UIElementState::HOVER;

        if (isMouseInScroll && mouseContext.isPressedThisFrame(sf::Mouse::Left) && !mouseContext.isConsumedThisFrame(sf::Mouse::Left)) {
            if (m_context->overflow[0] == UIOverflowMode::SCROLL && 
                    point_inversed.y - m_context->anchorBounds.m_pos.y > m_context->mouseBounds.m_height) {
                m_context->mousePressedOnScroll[0] = true;
                mouseContext.consume(sf::Mouse::Left);
            }
            if (m_context->overflow[1] == UIOverflowMode::SCROLL && 
                    point_inversed.x - m_context->anchorBounds.m_pos.x > m_context->mouseBounds.m_width) {
                m_context->mousePressedOnScroll[1] = true;
                mouseContext.consume(sf::Mouse::Left);
            }
        }
        if (mouseContext.isReleasedThisFrame(sf::Mouse::Left)) {
            if (m_context->mousePressedOnScroll[0] || m_context->mousePressedOnScroll[1]) {
                mouseContext.m_current.isConsumed[sf::Mouse::Left] = false;
            }
            m_context->mousePressedOnScroll[0] = false;
            m_context->mousePressedOnScroll[1] = false;
        }

        if (m_context->mousePressedOnScroll[0]) {
            state = UIElementState::ACTIVE;
            const float scrollbar_width = m_context->mouseBounds.m_width * (m_context->mouseBounds.m_width / std::max(m_context->mouseBounds.m_width, m_context->scrollCalulated[0]));
            const float ratio = std::max(0.0f, std::min(1.0f, (point_inversed.x - m_context->anchorBounds.m_pos.x - 0.5f * scrollbar_width) / (m_context->anchorBounds.m_width - scrollbar_width)));
            m_context->scrollPosX.setInstantly(ratio * (m_context->scrollCalulated[0] * (1.0f - scrollbar_width / m_context->mouseBounds.m_width)));
        }
        if (m_context->mousePressedOnScroll[1]) {
            state = UIElementState::ACTIVE;
            const float scrollbar_height = m_context->mouseBounds.m_height * (m_context->mouseBounds.m_height / std::max(m_context->mouseBounds.m_height, m_context->scrollCalulated[1]));
            const float ratio = std::max(0.0f, std::min(1.0f, (point_inversed.y - m_context->anchorBounds.m_pos.y - 0.5f * scrollbar_height) / (m_context->anchorBounds.m_height - scrollbar_height)));
            m_context->scrollPosY.setInstantly(ratio * (m_context->scrollCalulated[1] * (1.0f - scrollbar_height / m_context->mouseBounds.m_height)));
        }

        if (isMouseIn && mouseContext.m_current.mouseWheelScrolled) {
            mouseContext.m_current.mouseWheelScrolled = false;
            if (m_context->overflow[1] == UIOverflowMode::SCROLL) {
                const float scrollbar_height = m_context->mouseBounds.m_height * (m_context->mouseBounds.m_height / std::max(m_context->mouseBounds.m_height, m_context->scrollCalulated[1]));
                m_context->scrollPosY = std::max(
                    0.0f, 
                    std::min(
                        m_context->scrollPosY.getActual() - m_context->scrollWheelDistance * mouseContext.m_current.scrollDelta,
                        m_context->scrollCalulated[1] * (1.0f - scrollbar_height / m_context->mouseBounds.m_height)));
            }
            else {
                const float scrollbar_width = m_context->mouseBounds.m_width * (m_context->mouseBounds.m_width / std::max(m_context->mouseBounds.m_width, m_context->scrollCalulated[0]));
                m_context->scrollPosX = std::max(
                    0.0f, 
                    std::min(
                        m_context->scrollPosX.getActual() - m_context->scrollWheelDistance * mouseContext.m_current.scrollDelta,
                        m_context->scrollCalulated[0] * (1.0f - scrollbar_width / m_context->mouseBounds.m_width)));
            }
        }

        m_vars.scrollColor.stateChanged(state);
        m_vars.scrollBackgroundColor.stateChanged(state);
    }

    if (m_context->mousePressedHere || m_context->pushed) {
        m_context->m_current.state = UIElementState::ACTIVE;
    } else if (isMouseIn) {
        m_context->m_current.state = UIElementState::HOVER;
    } else {
        m_context->m_current.state = UIElementState::NORMAL;
    }
    if (m_context->pressable && m_context->pressed) {
        m_context->m_current.state = UIElementState::PRESSED;
    }
    if (m_context->focusable && m_context->focused) {
        m_context->m_current.state = UIElementState::FOCUSED;
    }

    if (m_context->m_current.state == UIElementState::ACTIVE) {
        m_context->triggerEvents(m_context->whileActive);
    }

    if (m_context->wasStateChanged()) {
        m_context->triggerEvents(m_context->onStateChange[size_t(m_context->m_current.state)]);
    }

    for (IUIAnimation* anim : m_animations) {
        anim->update();
    }
}

bool UIElement::checkDirtyBounds() {
    if (m_context->dirtyBounds) 
        return true;

    const bool isDirty =    m_vars.size.absolute.m_var.m_var.wasValueChanged() || 
                            m_vars.size.relative.m_var.m_var.wasValueChanged() ||
                            m_vars.pos.absolute.m_var.m_var.wasValueChanged() ||
                            m_vars.pos.relative.m_var.m_var.wasValueChanged() || 
                            m_vars.origin.absolute.m_var.m_var.wasValueChanged() || 
                            m_vars.origin.relative.m_var.m_var.wasValueChanged() || 
                            m_vars.flex.m_var.m_var.wasValueChanged() || 
                            m_vars.flex_order.m_var.m_var.wasValueChanged() ||
                            (m_context->m_prev.state == UIElementState::DISABLED && m_context->m_current.state != UIElementState::DISABLED) ||
                            (m_context->m_prev.state != UIElementState::DISABLED && m_context->m_current.state == UIElementState::DISABLED) ||
                            m_anchor->m_context->dirtyBounds;
    
    if (isDirty) {
        m_context->dirtyBounds = true;
        return true;
    }
    if (m_context->scrollPosX.wasValueChanged() || m_context->scrollPosY.wasValueChanged()) {
        m_context->dirtyBounds = true;
        return false;
    }
    
    bool childDirty = false;
    for (UIElement* child : m_children) {
        childDirty = childDirty || child->checkDirtyBounds();
    }
    if (childDirty && (
            m_context->sizeMode[0] == UISizeMode::FIT_CONTENT || 
            m_context->sizeMode[1] == UISizeMode::FIT_CONTENT) &&
            !m_context->fitContentFixed) {
        m_context->dirtyBounds = true;
        return true;
    }
    return false;
}
void UIElement::checkDirtyTransforms() {
    const bool isDirty =    m_vars.translate.absolute.m_var.m_var.wasValueChanged() || 
                            m_vars.translate.relative.m_var.m_var.wasValueChanged() ||
                            m_vars.rotate.m_var.m_var.wasValueChanged() ||
                            m_vars.scale.m_var.m_var.wasValueChanged() || 
                            m_vars.opacity.m_var.m_var.wasValueChanged();
    
    if (isDirty) {
        m_context->dirtyTransform = true;
        return;
    }
    for (UIElement* child : m_children) {
        child->checkDirtyTransforms();
    }
}

void UIElement::setState(UIElementState state) {
    m_context->m_current.state = state;
    checkChangedStates();
}

void UIElement::checkChangedStates() {
    m_vars.checkChangedStates();
}

void UIElement::addChild(UIElement* child, UIElement* anchor) {
    child->m_parent = this;
    child->m_anchor = anchor ? anchor : this;
    child->m_context->z_index = m_context->z_index + 1;
    m_children.push_back(child);
}

UIElement* UIElement::getChildById(std::string id) {
    if (m_context->id == id) {
        return this;
    }
    for (UIElement* c : m_children) {
        UIElement* found = c->getChildById(id);
        if (found != nullptr) {
            return found;
        } 
    }
    return nullptr;
}