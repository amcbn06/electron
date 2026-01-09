#pragma once
// Standard Library headers
#include <vector>
// Third-Party headers
#include <SFML/Graphics.hpp>
// Project headers
#include "Component.hpp"

namespace Renderer {
    // Fundamental shapes
    void drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b, sf::Color color = sf::Color::White);

    void drawRectangle(sf::RenderWindow& window, sf::Vector2f upperLeft, sf::Vector2f lowerRight, sf::Color color = sf::Color::White);

    void drawEllipse(sf::RenderWindow& window, sf::Vector2f center, float a, float b, sf::Color color, bool full = false, int segments = 64);
    
    // Project related
    void drawGrid(sf::RenderWindow& window, const sf::View& view);

    void drawComponent(sf::RenderWindow& window, const Component& comp);

    void drawMenu(sf::RenderWindow& window);

    void drawAllComponents(sf::RenderWindow& window);

    void drawWires(sf::RenderWindow& window);
}