#include "Utils.hpp"

#include <cmath>

#include "Constants.hpp"

namespace Utils {
    void rotatePoint(sf::Vector2f origin, sf::Vector2f& point, float angleDegrees) {
        float rad = angleDegrees * Constants::PI / 180.0f;
        float s = std::sin(rad);
        float c = std::cos(rad);

        sf::Vector2f rel = point - origin;
        sf::Vector2f rotated(
            rel.x * c - rel.y * s,
            rel.x * s + rel.y * c
        );

        point = origin + rotated;
    }
}