#pragma once
#include "EngineContext.hpp"
#include "../UI/UIManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>

class Engine {
public:
    Engine();

    void createWindowHandler(
        sf::VideoMode mode, const sf::String &title,
        sf::Uint32 style = 7U, const sf::ContextSettings &settings = sf::ContextSettings());
    void createSimulation();
    void createUIManager();
    
    void startFrame();
    void endFrame();
    void handleEvent(sf::Event& e);
    void update(float dt);
    void draw();

    EngineContext m_context;
    UIManager* m_UIManager;
};