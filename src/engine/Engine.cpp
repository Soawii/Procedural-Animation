#include "Engine.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include "../util/util.hpp"

Engine::Engine() : m_context() {
}

void Engine::createWindowHandler(
        sf::VideoMode mode, const sf::String &title,
        sf::Uint32 style, const sf::ContextSettings &settings) {
    m_context.m_window = new sf::RenderWindow(mode, title, style, settings);
}
void Engine::createSimulation() {
    m_context.m_sim = new Simulation();
}
void Engine::createUIManager() {
    m_UIManager = new UIManager(m_context);
}

void Engine::startFrame() {
    conf::time::now = std::chrono::steady_clock::now();
    m_UIManager->startFrame();
}
void Engine::endFrame() {
    m_UIManager->endFrame();
}
void Engine::handleEvent(sf::Event& e) {
    if (e.type == sf::Event::Closed) {
        m_context.m_window->close();
    }
    else if (e.type == sf::Event::Resized) {
        m_context.m_window->setSize({e.size.width, e.size.height});
        const sf::View view(sf::FloatRect(0.0f, 0.0f, e.size.width, e.size.height));
        m_context.m_window->setView(view);
        conf::window::WIDTH = e.size.width;
        conf::window::HEIGHT = e.size.height;
    }
    m_UIManager->handleEvent(e);
}
void Engine::update(float dt) {
    m_context.m_sim->update(dt);
    m_UIManager->update();
}

void Engine::draw() {
    m_context.m_window->clear(conf::colors::backgroundColor);
    m_context.m_sim->draw(m_context.m_window);
    m_UIManager->draw();
    m_context.m_window->display();
}