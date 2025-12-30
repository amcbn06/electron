#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using ComponentType = std::string;

const std::vector<ComponentType> types = {
    "servo_motor",
    "baterie",
    "condensator_polarizat",
    "dioda_zenner",
    "tranzistor_npn",
    "nod",
    "nand",
    "stop",
    "condensator",
    "tranzistor_pnp",
    "rezistenta",
    "dioda",
    "amplificator"
};

struct Component {
    ComponentType type;
    sf::Vector2f position;
    float rotation = 0.0f; // In degrees
    bool isSelected = false;
};

// Simplified spawn function (no texture needed)
void spawnComponent(std::vector<Component>& list, ComponentType type, sf::Vector2f pos);