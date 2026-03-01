#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    inline const sf::Color Background  = sf::Color(30, 30, 30);
    inline const sf::Color GridDots    = sf::Color(100, 100, 100);
    inline const sf::Color GridLines   = sf::Color(50, 50, 50);

    inline const sf::Color Component   = sf::Color(200, 200, 200);

    inline const sf::Color Selection   = sf::Color(66, 135, 245, 100);

    namespace Wire {
        inline const float cornerRadius = 3.0f;
        inline const sf::Color idle = sf::Color::White;
        inline const sf::Color ghost = sf::Color(105, 105, 105);
    }

    namespace Pin {
        inline const sf::Color idle = sf::Color(105, 105, 105);
        inline const sf::Color selected = sf::Color::Red;
    }
}