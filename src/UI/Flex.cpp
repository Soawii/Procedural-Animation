#include "Flex.hpp"

Flex::Flex() {} 

void Flex::computeSize() {
    if (m_context->sizeMode[0] == UISizeMode::FIXED ||
        m_context->sizeMode[1] == UISizeMode::FIXED) {
        computeBounds();
    }

    for (UIElement* child : m_children) {
        child->computeSize();
    }

    std::vector<UIElement*> sortedChildren = m_children;
    std::stable_sort(sortedChildren.begin(), sortedChildren.end(), [](UIElement* a, UIElement* b) {
        return a->m_vars.flex_order.get() < b->m_vars.flex_order.get();
    });
    if (m_reverse) {
        std::reverse(sortedChildren.begin(), sortedChildren.end());
    }

    int childrenAmount = 0;
    float totalMain = 0.0f;
    float maxCross = 0.0f;

    std::vector<sf::Vector2f> childBounds;
    std::vector<float> childFlex;

    for (UIElement* child : sortedChildren) {
        const sf::Vector2f cb = child->getLocalBounds();
        const float flex = child->m_vars.flex.get();
        childBounds.push_back(cb);
        childFlex.push_back(flex);

        if (child->m_context->m_current.state == UIElementState::DISABLED) {
            continue;
        }

        childrenAmount++;

        if (m_direction == FlexDirection::horizontal) {
            if (flex == 0.0f)
                totalMain += cb.x;
            maxCross = std::max(maxCross, cb.y);
        } else {
            if (flex == 0.0f)
                totalMain += cb.y;
            maxCross = std::max(maxCross, cb.x);
        }
    }
    const float totalGaps = (childrenAmount > 1 ? m_gap * (childrenAmount - 1) : 0.0f);

    if (m_context->sizeMode[0] == UISizeMode::FIT_CONTENT ||
        m_context->sizeMode[1] == UISizeMode::FIT_CONTENT) {

        const float borderWidth = m_vars.borderWidth.get();
        const float padding = m_vars.padding.get();

        const sf::Vector2f size = m_vars.size.absolute.get();
        sf::Vector2f newSize = size;

        if (m_context->sizeMode[0] == UISizeMode::FIT_CONTENT) {
            newSize.x = (m_direction == FlexDirection::horizontal ? (totalMain + totalGaps) : maxCross)
                + borderWidth * 2 + padding * 2;
        }
        if (m_context->sizeMode[1] == UISizeMode::FIT_CONTENT) {
            newSize.y = (m_direction == FlexDirection::vertical ? (totalMain + totalGaps) : maxCross)
                + borderWidth * 2 + padding * 2;
        }

        if (size != newSize) {
            m_vars.size.absolute.setAllSmoothly(newSize);
            computeBounds();
        }
    }

    const Bounds& contentBounds = m_context->childrenBounds;

    const float containerMainSize = (m_direction == FlexDirection::horizontal) ? contentBounds.m_width : contentBounds.m_height;
    const float containerCrossSize = (m_direction == FlexDirection::horizontal) ? contentBounds.m_height : contentBounds.m_width;

    float remainingSpace = containerMainSize - totalMain;
    float startOffset = 0.0f;
    float spacing = m_gap;

    float totalFlex = 0.0f;
    for (int i = 0; i < sortedChildren.size(); i++) {
        totalFlex += childFlex[i];
    }
    if (totalFlex > 0.0f) {
        const float availableMain = containerMainSize - (totalMain + totalGaps);
        for (int i = 0; i < sortedChildren.size(); i++) {
            UIElement* child = sortedChildren[i];
            if (child->m_context->m_current.state == UIElementState::DISABLED) {
                continue;
            }
            const float flexValue = childFlex[i];
            if (flexValue > 0.0f) {
                const sf::Vector2f childSize = child->m_vars.size.absolute.get();
                sf::Vector2f newChildSize = childSize;
                if (m_direction == FlexDirection::horizontal) {
                    newChildSize.x = (flexValue / totalFlex) * availableMain;
                } else {
                    newChildSize.y = (flexValue / totalFlex) * availableMain;
                }
                if (childSize != newChildSize) {
                    child->m_vars.size.absolute.setAll(newChildSize);
                    childBounds[i] = child->getLocalBounds();
                }
            }
        }
        remainingSpace = totalGaps;
    }

    switch (m_justify) {
        case JustifyContent::start:
            break;
        case JustifyContent::end:
            startOffset = remainingSpace - totalGaps;
            break;
        case JustifyContent::center:
            startOffset = (remainingSpace - totalGaps) / 2.0f;
            break;
        case JustifyContent::space_between:
            spacing = (childrenAmount > 1) ? (remainingSpace / (childrenAmount - 1)) : 0.0f;
            break;
        case JustifyContent::space_around:
            spacing = (childrenAmount > 0) ? (remainingSpace / childrenAmount) : 0.0f;
            startOffset = spacing / 2.0f;
            break;
    }

    float currentPos = startOffset;
    for (int i = 0; i < sortedChildren.size(); i++) {
        UIElement* child = sortedChildren[i];
        if (child->m_context->m_current.state == UIElementState::DISABLED) {
            continue;
        }
        const sf::Vector2f& cb = childBounds[i];

        const sf::Vector2f childPos = child->m_vars.size.absolute.get();
        sf::Vector2f newChildPos = childPos;

        if (m_direction == FlexDirection::horizontal) {
            newChildPos.x = currentPos;
            switch (m_align) {
                case AlignItems::start:
                    newChildPos.y = 0.0f;
                    break;
                case AlignItems::end:
                    newChildPos.y = containerCrossSize - cb.y;
                    break;
                case AlignItems::center:
                    newChildPos.y = (containerCrossSize - cb.y) / 2.0f;
                    break;
            }
            currentPos += cb.x + spacing;
        } else {
            newChildPos.y = currentPos;
            switch (m_align) {
                case AlignItems::start:
                    newChildPos.x = 0.0f;
                    break;
                case AlignItems::end:
                    newChildPos.x = containerCrossSize - cb.x;
                    break;
                case AlignItems::center:
                    newChildPos.x = (containerCrossSize - cb.x) / 2.0f;
                    break;
            }
            currentPos += cb.y + spacing;
        }
        if (childPos != newChildPos)
            child->m_vars.pos.absolute.setAllSmoothly(newChildPos);
    }
}

bool Flex::checkDirtyBounds() {
    const bool isDirty =    m_vars.size.absolute.m_var.m_var.wasValueChanged() || 
                            m_vars.size.relative.m_var.m_var.wasValueChanged() ||
                            m_vars.pos.absolute.m_var.m_var.wasValueChanged() ||
                            m_vars.pos.relative.m_var.m_var.wasValueChanged() || 
                            m_vars.origin.absolute.m_var.m_var.wasValueChanged() || 
                            m_vars.origin.relative.m_var.m_var.wasValueChanged() || 
                            m_vars.flex.m_var.m_var.wasValueChanged() || 
                            m_vars.flex_order.m_var.m_var.wasValueChanged() || 
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
    if (childDirty) {
        m_context->dirtyBounds = true;
        return true;
    }
    return false;
}