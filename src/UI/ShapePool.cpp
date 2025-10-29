#include "ShapePool.hpp"
#include <iostream>
  
RectParams::RectParams(float width_, float height_) 
: width(width_), height(height_) {}
bool RectParams::operator==(const RectParams& o) const
{
    return width == o.width && height == o.height;
}  
size_t RectParamsHasher::operator()(const RectParams& s) const {
    return (s.width * size_t(10000)) + s.height;
}

RoundedRectParams::RoundedRectParams(float width_, float height_, float borderRadius_) 
: width(width_), height(height_), borderRadius(borderRadius_) {}
bool RoundedRectParams::operator==(const RoundedRectParams& o) const
{
    return width == o.width && height == o.height && borderRadius == o.borderRadius;
}  
size_t RoundedRectParamsHasher::operator()(const RoundedRectParams& s) const {
    return ((s.width * size_t(10000)) + s.height) * size_t(1000) + s.borderRadius;
}

RoundedOutlinedRectParams::RoundedOutlinedRectParams(float width_, float height_, float borderWidth_, float borderRadius_) 
: width(width_), height(height_), borderWidth(borderWidth_), borderRadius(borderRadius_) {}
bool RoundedOutlinedRectParams::operator==(const RoundedOutlinedRectParams& o) const
{
    return width == o.width && height == o.height && borderWidth == o.borderWidth && borderRadius == o.borderRadius;
}  
size_t RoundedOutlinedRectParamsHasher::operator()(const RoundedOutlinedRectParams& s) const {
    return ((s.width * size_t(10000) + s.height) * size_t(1000) + s.borderWidth) * size_t(1000) + s.borderRadius;
}

std::unordered_map<RectParams, shapes::Rect*, RectParamsHasher> ShapePool::m_rects;
std::unordered_map<RoundedRectParams, shapes::RoundedRect*, RoundedRectParamsHasher> ShapePool::m_roundedRects;
std::unordered_map<RoundedOutlinedRectParams, shapes::RoundedOutlinedRect*, RoundedOutlinedRectParamsHasher> ShapePool::m_roundedOutlinedRects;

void ShapePool::addShape(const RectParams& p, shapes::Rect* shape) {
    m_rects[p] = shape;
}
void ShapePool::addShape(const RoundedRectParams& p, shapes::RoundedRect* shape) {
    m_roundedRects[p] = shape;
}
void ShapePool::addShape(const RoundedOutlinedRectParams& p, shapes::RoundedOutlinedRect* shape) {
    m_roundedOutlinedRects[p] = shape;
}

shapes::Rect* ShapePool::getRect(const RectParams& p, const sf::Color& fillColor) {
    const auto it = m_rects.find(p);
    if (it == m_rects.end())
        return nullptr;
    shapes::Rect* shape = it->second;
    if (shape->m_arr.arr[0].color != fillColor) {
        for (size_t i = 0; i < shape->m_arr.arr.getVertexCount(); i++) {
            shape->m_arr.arr[i].color = fillColor;
        }
    }
    return shape;
}
shapes::Rect* ShapePool::getAddRect(const RectParams& p, const sf::Color& fillColor) {
    shapes::Rect* shape = getRect(p, fillColor);
    if (shape == nullptr) {
        shape = new shapes::Rect({0.0f, 0.0f}, p.width, p.height, fillColor);
        addShape(p, shape);
    }
    return shape;
}
shapes::RoundedRect* ShapePool::getRoundedRect(const RoundedRectParams& p, const sf::Color& fillColor) {
    const auto it = m_roundedRects.find(p);
    if (it == m_roundedRects.end())
        return nullptr;
    shapes::RoundedRect* shape = it->second;
    if (shape->m_arr.arr[0].color != fillColor) {
        for (size_t i = 0; i < shape->m_arr.arr.getVertexCount(); i++) {
            shape->m_arr.arr[i].color = fillColor;
        }
    }
    return shape;
}
shapes::RoundedRect* ShapePool::getAddRoundedRect(const RoundedRectParams& p, const sf::Color& fillColor) {
    shapes::RoundedRect* shape = getRoundedRect(p, fillColor);
    if (shape == nullptr) {
        shape = new shapes::RoundedRect({0.0f, 0.0f}, p.width, p.height, p.borderRadius, 5, fillColor);
        addShape(p, shape);
    }
    return shape;
}
shapes::RoundedOutlinedRect* ShapePool::getRoundedOutlinedRect(const RoundedOutlinedRectParams& p, const sf::Color& fillColor, const sf::Color& borderColor) {
    const auto it = m_roundedOutlinedRects.find(p);
    if (it == m_roundedOutlinedRects.end())
        return nullptr;
    shapes::RoundedOutlinedRect* shape = it->second;
    if (shape->m_innerRect.arr[0].color != fillColor) {
        for (size_t i = 0; i < shape->m_innerRect.arr.getVertexCount(); i++) {
            shape->m_innerRect.arr[i].color = fillColor;
        }
    }
    if (shape->m_outline.arr[0].color != borderColor) {
        for (size_t i = 0; i < shape->m_outline.arr.getVertexCount(); i++) {
            shape->m_outline.arr[i].color = borderColor;
        }
    }
    return shape;
}
shapes::RoundedOutlinedRect* ShapePool::getAddRoundedOutlinedRect(const RoundedOutlinedRectParams& p, const sf::Color& fillColor, const sf::Color& borderColor) {
    shapes::RoundedOutlinedRect* shape = getRoundedOutlinedRect(p, fillColor, borderColor);
    if (shape == nullptr) {
        shape = new shapes::RoundedOutlinedRect({0.0f, 0.0f}, p.width, p.height, p.borderRadius, fillColor, p.borderWidth, borderColor, 5);
        addShape(p, shape);
    }
    return shape;
}