#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

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

struct Wire {
    int startComponentIndex;
    int startPinIndex;
    int endComponentIndex;
    int endPinIndex;
    bool operator==(const Wire& other) const {
        return startComponentIndex == other.startComponentIndex &&
               startPinIndex == other.startPinIndex &&
               endComponentIndex == other.endComponentIndex &&
               endPinIndex == other.endPinIndex;
    }
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
    bool sters = false;

    
    std::vector<sf::Vector2f> relPins;

    Component() { }

    Component(float x, float y, ComponentType type, bool simple = false, float zoom = 1.0f);

    sf::Vector2f getAbsPin(int pinIndex) const;
};

extern std::vector<Component> components;
extern std::vector<Wire> wires;

void spawnComponent(ComponentType type, sf::Vector2f pos);

int getSelection();

bool tooClose(sf::Vector2f pos, int index);

int findClosest(sf::Vector2f pos);




std::pair<int, int> findPinAt(const sf::Vector2f& mousePos);

void rotate(Component& x);

void editMf(Component& x);

void zoom(Component& x, bool zoomIn);