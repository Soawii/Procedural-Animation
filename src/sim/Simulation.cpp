#include "Simulation.hpp"

Simulation::Simulation() {

}

void Simulation::update(float dt) {
    m_entities[m_chosen_entity]->update(dt);
}

void Simulation::draw(sf::RenderWindow* window) {
    m_entities[m_chosen_entity]->draw(window);
}