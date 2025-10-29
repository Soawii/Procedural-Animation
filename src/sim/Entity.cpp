#include "Entity.hpp"

Entity::Entity() {
    
}

void Entity::update(float dt) {
    
}

void Entity::draw(sf::RenderWindow* window) {
    sf::CircleShape target(5.0f, 10);
    target.setFillColor(sf::Color::Red);
    target.setOrigin(5.0f, 5.0f);
    target.setPosition(m_targetPoint);
    window->draw(target);
}
