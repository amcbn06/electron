#include "Component.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

#include "Constants.hpp"
#include "Utils.hpp"

std::vector<Component> components;
std::vector<Wire> wires;

Component::Component(float x, float y, ComponentType type, bool simple, float zoom) : type(type), simple(simple) {
    position = sf::Vector2f(x, y);
    tie(unit, value, scale) = (*Constants::componente.find(type)).second;
    scale *= zoom;

    // load pins from template files immediately
    std::ifstream fin("assets/" + type + ".txt");
    std::string dummy;
    std::getline(fin, dummy);

    int pinCount;
    fin >> pinCount;
    relPins.resize(pinCount);
    for(int i = 0; i < pinCount; ++i){
        fin >> relPins[i].x >> relPins[i].y;
    }

    fin.close();
}

sf::Vector2f Component::getAbsPin(int pinIndex) {
    assert(relPins.size() > 0 && pinIndex >= 0 && pinIndex < relPins.size());
    sf::Vector2f absPin = relPins[pinIndex];
    absPin = absPin * scale + position;
    Utils::rotatePoint(position, absPin, rotation);
    return absPin;
}

std::pair<int, int> findPinAt(const sf::Vector2f& pos) {
    std::pair<int, int> closestPin(-1, -1);
    float minDistance = INFINITY;
    for (int i = 0; i < components.size(); ++i) {
        for (int j = 0; j < components[i].relPins.size(); ++j) {
            sf::Vector2f absPin = components[i].getAbsPin(j);
            sf::Vector2f delta = absPin - pos;
            float distance = std::hypot(delta.x, delta.y);
            // ignore pins that are too far
            if (distance > Constants::Pin::clickTolerance) {
                continue;
            }
            // keep only the closest pin
            if (distance < minDistance) {
                closestPin = std::make_pair(i, j);
                minDistance = distance;
            }
        }
    }
    return closestPin;
}


void spawnComponent(ComponentType type, sf::Vector2f pos) {
    components.emplace_back(pos.x, pos.y, type);
    if (components.size() >= 2) {
        std::cerr << "adding a wire" << std::endl;
        wires.push_back(Wire{
            (int)components.size() - 2,
            0,
            (int)components.size() - 1,
            0
        });
    }
}

int findClosest(sf::Vector2f pos) {
    int index = -1;
    float minDist = INFINITY;
    for (int i = 0; i < components.size(); ++i) {
        float d = std::hypot(components[i].position.x - pos.x, components[i].position.y - pos.y);
        if (d < minDist) {
            minDist = d;
            index = i;
        }
    }
    if (minDist < Constants::select_thr) {
        return index;
    }
    return -1;
}



bool tooClose(sf::Vector2f pos, int index) {
    for (int i = 0; i < components.size(); ++i) {
        if (i == index) {
            continue;
        }
        float d = std::hypot(components[i].position.x - pos.x, components[i].position.y - pos.y);
        if (d < Constants::min_dist_comps) {
            return true;
        }
    }
    return false;
}

int getSelection() {
    for (int i = 0; i < components.size(); ++i) {
        if (components[i].isSelected)
        {
            return i;
        }
    }
    return -1;
}

void rotate(Component& component) {
    component.rotation = (static_cast<int>(component.rotation) + 90) % 360;
}

void editMf(Component& component) {
    float value;
    std::cout << "Introdu value noua: ";
    std::cin >> value;
    component.value = value;
}

void zoom(Component& component, bool zoomIn) {
    float zoom = 1.0f;
    if (zoomIn) {
        zoom += Constants::zoomSensitivity;
    }
    else {
        zoom -= Constants::zoomSensitivity;
    }

    auto it = (*Constants::componente.find(component.type)).second;

    float initial_scale = std::get<2>(it);

    // clamp zoom level
    if (component.scale * zoom < initial_scale / Constants::zoomAlpha
        && component.scale * zoom > initial_scale * Constants::zoomAlpha) {
        component.scale *= zoom;
    }
}