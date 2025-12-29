#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>

void spawnComponent(std::vector<Component>& list, ComponentType type, sf::Vector2f pos) {
    Component c;
    c.type = type;

    using Constants::gridStep;
    
    // Snap to nearest grid intersection
    c.position.x = std::round(pos.x / gridStep) * gridStep;
    c.position.y = std::round(pos.y / gridStep) * gridStep;
    
    // Default rotation
    c.rotation = 0.0f;
    c.isSelected = false;

    list.push_back(c);
}