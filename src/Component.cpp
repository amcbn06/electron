#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>

void spawnComponent(std::vector<Component>& list, ComponentType type, sf::Vector2f pos) {
    using Constants::gridStep;

    type = types[rand()%types.size()];

    list.push_back(Component{
        std::round(pos.x / gridStep) * gridStep,        
        std::round(pos.y / gridStep) * gridStep,
        type,
        false
    });
}