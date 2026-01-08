#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "Constants.hpp"

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
    "amplificator"
};

struct Component {
    ComponentType type;
    sf::Vector2f position;

    std::string unit;
    float value;

    float rotation = 0.0f;
    float scale = 20.0f;
    bool isSelected = false;
    bool simple = false;

    Component(float x, float y, ComponentType type, bool simple = false, float zoom = 1.0f) : type(type), simple(simple) {
        position = sf::Vector2f(x, y);
        auto [name, value, myScale] = (*Constants::componente.find(type)).second;
        scale = myScale * zoom;
        unit = name;
        value = value;
        // std::cerr << "Spawned component of type \"" << type << "\" with value " << value << " " << unit << std::endl;
    }
    Component() {}
};

void spawnComponent(ComponentType type, sf::Vector2f pos);

int getSelection();

bool tooClose(sf::Vector2f pos, int index);

int findClosest(sf::Vector2f pos);

void rotate(Component& x);

void editMf(Component& x);

void zoom(Component& x, bool direction);