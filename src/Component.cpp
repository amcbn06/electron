#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>
#include <sstream>

std::vector<Component> components;

void spawnComponent(ComponentType type, sf::Vector2f pos)
{
    using Constants::gridStep;

    components.push_back(Component{
        std::round(pos.x / gridStep) * gridStep,
        std::round(pos.y / gridStep) * gridStep,
        type,
        false});
}

int find_closest(sf::Vector2f pos)
{
    int qui = -1;
    float minDist = INFINITY;
    for (int i = 0; i < components.size(); ++i)
    {
        float d = std::hypot(components[i].position.x - pos.x, components[i].position.y - pos.y);
        if (d < minDist)
        {
            minDist = d;
            qui = i;
        }
    }
    if (minDist < Constants::select_thr)
    {
        return qui;
    }
    return -1;
}

bool too_close(sf::Vector2f pos, int qui)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (i == qui)
            continue;
        float d = std::hypot(components[i].position.x - pos.x, components[i].position.y - pos.y);
        if (d < Constants::min_dist_comps)
        {
            return true;
        }
    }
    return false;
}

int get_selection()
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i].isSelected)
        {
            return i;
        }
    }
    return -1;
}

void rotate(Component &x)
{
    x.rotation += 90;
    if (x.rotation == 360)
    {
        x.rotation = 0;
    }
}

void editMf(Component &x)
{
    float new_val;
    std::cout << "Introdu valoare noua: ";
    std::cin >> new_val;
    x.valoare = new_val;
}

void zoom(Component &x, bool direction)
{
    float zoom = 1.0f;
    if (direction)
    {
        zoom += Constants::zoomSensitivity;
    }
    else
    {
        zoom -= Constants::zoomSensitivity;
    }

    auto it = (*Constants::componente.find(x.type)).second;

    float initial_scale = std::get<2>(it);

    if (x.scale * zoom < initial_scale / Constants::zoomAlpha && x.scale * zoom > initial_scale * Constants::zoomAlpha)
    {
        x.scale *= zoom;
    }
}