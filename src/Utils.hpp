#pragma once
#include <SFML/Graphics.hpp>

namespace Utils {
    // Rotates a point around an origin by a specific angle (in degrees)
    void rotatePoint(sf::Vector2f origin, sf::Vector2f& point, float angleDegrees);
}