#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    // Background and Grid
    inline const sf::Color Background  = sf::Color(30, 30, 30);
    // dark gray sf::Color(30, 30, 30)
    // greyish dark blue sf::Color(27, 25, 79)
    inline const sf::Color GridDots    = sf::Color(100, 100, 100);
    inline const sf::Color GridLines   = sf::Color(50, 50, 50);

    // Logic Colors
    inline const sf::Color WireNormal  = sf::Color(0, 255, 0);   // Green
    inline const sf::Color WireActive  = sf::Color(255, 255, 0); // Yellow
    inline const sf::Color Component   = sf::Color(200, 200, 200);
    
    // UI
    inline const sf::Color Selection   = sf::Color(66, 135, 245, 100); // Semi-transparent blue
}