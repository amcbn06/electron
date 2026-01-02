#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Constants.hpp"
#include <map>

using ComponentType = std::string;

const std::vector<ComponentType> types = {
    "servo_motor",
    "baterie",
    "condensator_polarizat",
    "dioda_zenner",
    "tranzistor_npn",
    "nand",
    "condensator",
    "tranzistor_pnp",
    "rezistenta",
    "dioda",
    "amplificator"};

struct Component
{
    ComponentType type;
    sf::Vector2f position;

    std::string marime_fizica;
    float valoare;

    float rotation = 0.0f; // In degrees
    float scale = 20.0f;
    bool isSelected = false;
    bool simple = false;

    Component(int x, int y, ComponentType type, bool simple = false, float zoom = 1.0f) : type(type), simple(simple)
    {
        position = {x, y};
        auto [name, value, myScale] = (*Constants::componente.find(type)).second;
        scale = myScale * zoom;
        marime_fizica = name;
        valoare = value;
    }
    Component() {}
};

// Simplified spawn function (no texture needed)
void spawnComponent(std::vector<Component> &list, ComponentType type, sf::Vector2f pos);