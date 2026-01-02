#pragma once
#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include <vector>

namespace Renderer {
    void drawGrid(sf::RenderWindow& window, const sf::View& view);

    sf::Vector2f rotatePoint(sf::Vector2f point, float angleDegrees);

    void drawComponent(sf::RenderWindow& window, const Component& comp);

    void drawAllComponents(sf::RenderWindow& window, const std::vector<Component>& list);

    void drawMenu(sf::RenderWindow& window);
}