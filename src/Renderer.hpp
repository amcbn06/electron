#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Component.hpp"

namespace Renderer {
    
    void drawLine(sf::RenderWindow&, sf::Vector2f, sf::Vector2f, sf::Color = sf::Color::White);

    void drawRectangle(sf::RenderWindow&, sf::Vector2f, sf::Vector2f, sf::Color = sf::Color::White);

    void drawEllipse(sf::RenderWindow&, sf::Vector2f, float, float, sf::Color, bool = false, int = 64);
    
    
    void drawGrid(sf::RenderWindow&, const sf::View&);

    void drawAutoRoute(sf::RenderWindow&, sf::Vector2f, sf::Vector2f,  sf::Vector2f, float, sf::Vector2f, float, sf::Color color);

    void drawWires(sf::RenderWindow&);

    void drawComponent(sf::RenderWindow&, const Component&);

    void drawAllComponents(sf::RenderWindow&);

    void drawMenu(sf::RenderWindow&);
}