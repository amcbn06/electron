#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum ComponentType { RESISTOR, CAPACITOR, LED };

struct Component {
    ComponentType type;
    sf::Vector2f position;
    float rotation = 0.0f; // In degrees
    bool isSelected = false;
};

// Simplified spawn function (no texture needed)
void spawnComponent(std::vector<Component>& list, ComponentType type, sf::Vector2f pos);