#pragma once
#include "../draw/Shapes.hpp"
#include <vector>
#include <unordered_map>

struct RectParams {
    size_t width, height;
    RectParams(float width_, float height_);
    bool operator==(const RectParams& o) const;
};    
struct RectParamsHasher {
    size_t operator()(const RectParams& s) const;
};

struct RoundedRectParams {
    size_t width, height, borderRadius;
    RoundedRectParams(float width_, float height_, float borderRadius_);
    bool operator==(const RoundedRectParams& o) const;
};    
struct RoundedRectParamsHasher {
    size_t operator()(const RoundedRectParams& s) const;
};

struct RoundedOutlinedRectParams {
    size_t width, height, borderWidth, borderRadius;
    RoundedOutlinedRectParams(float width_, float height_, float borderWidth_, float borderRadius_);
    bool operator==(const RoundedOutlinedRectParams& o) const;
};    
struct RoundedOutlinedRectParamsHasher {
    size_t operator()(const RoundedOutlinedRectParams& s) const;
};

class ShapePool {
public:     
    static std::unordered_map<RectParams, shapes::Rect*, RectParamsHasher> m_rects;
    static std::unordered_map<RoundedRectParams, shapes::RoundedRect*, RoundedRectParamsHasher> m_roundedRects;
    static std::unordered_map<RoundedOutlinedRectParams, shapes::RoundedOutlinedRect*, RoundedOutlinedRectParamsHasher> m_roundedOutlinedRects;

    static void addShape(const RectParams& p, shapes::Rect* shape);
    static void addShape(const RoundedRectParams& p, shapes::RoundedRect* shape);
    static void addShape(const RoundedOutlinedRectParams& p, shapes::RoundedOutlinedRect* shape);

    static shapes::Rect* getRect(const RectParams& p, const sf::Color& fillColor);
    static shapes::Rect* getAddRect(const RectParams& p, const sf::Color& fillColor);
    static shapes::RoundedRect* getRoundedRect(const RoundedRectParams& p, const sf::Color& fillColor);
    static shapes::RoundedRect* getAddRoundedRect(const RoundedRectParams& p, const sf::Color& fillColor);
    static shapes::RoundedOutlinedRect* getRoundedOutlinedRect(const RoundedOutlinedRectParams& p, const sf::Color& fillColor, const sf::Color& borderColor);
    static shapes::RoundedOutlinedRect* getAddRoundedOutlinedRect(const RoundedOutlinedRectParams& p, const sf::Color& fillColor, const sf::Color& borderColor);
};