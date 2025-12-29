#include "Renderer.hpp"
#include "Theme.hpp"
#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>

namespace Renderer {

    // Drawing the grid
    void drawGrid(sf::RenderWindow& window, const sf::View& view) {
        sf::Vector2f center = view.getCenter();
        sf::Vector2f size = view.getSize();
        
        float left = center.x - size.x / 2.0f;
        float top = center.y - size.y / 2.0f;
        
        using Constants::gridStep;

        float startX = std::floor(left / gridStep) * Constants::gridStep;
        float startY = std::floor(top / gridStep) * gridStep;

        sf::CircleShape dot(1.5f);
        dot.setRadius(1.5f);
        dot.setFillColor(Theme::GridDots);

        for (float x = startX; x <= left + size.x; x += gridStep) {
            for (float y = startY; y <= top + size.y; y += gridStep) {
                dot.setPosition(x, y);
                window.draw(dot);
            }
        }
    }

    // Helper to rotate a point around (0,0)
    sf::Vector2f rotatePoint(sf::Vector2f point, float angleDegrees) {
        float rad = angleDegrees * M_PI / 180.0f;
        float s = std::sin(rad);
        float c = std::cos(rad);
        return sf::Vector2f(point.x * c - point.y * s, point.x * s + point.y * c);
    }

    void drawResistor(sf::RenderWindow& window, const Component& comp) {
        // Define the shape relative to center (0,0)
        // A standard resistor has leads and a zig-zag body
        std::vector<sf::Vector2f> points = {
            {-30, 0}, {-15, 0},      // Left Wire
            {-10, -10}, {0, 10},     // Up, Down
            {10, -10}, {15, 0},      // Up, Back to center
            {30, 0}                  // Right Wire
        };

        sf::VertexArray lines(sf::LinesStrip, points.size());

        for (size_t i = 0; i < points.size(); i++) {
            // 1. Rotate the point based on component rotation
            sf::Vector2f rotated = rotatePoint(points[i], comp.rotation);
            
            // 2. Translate to the actual component position
            lines[i].position = rotated + comp.position;
            
            // 3. Set Color (Yellow if selected, White otherwise)
            lines[i].color = comp.isSelected ? sf::Color::Yellow : sf::Color::White;
        }

        window.draw(lines);
    }

    void drawCapacitor(sf::RenderWindow& window, const Component& comp) {
        // Capacitor is two parallel plates
        // We need discrete lines (sf::Lines), not a strip
        sf::VertexArray lines(sf::Lines);

        // Define pairs of points (Start -> End)
        std::vector<sf::Vector2f> rawPoints = {
            {-30, 0}, {-5, 0},   // Left Wire
            {-5, -15}, {-5, 15}, // Left Plate (Vertical)
            {5, -15}, {5, 15},   // Right Plate (Vertical)
            {5, 0}, {30, 0}      // Right Wire
        };

        for (const auto& p : rawPoints) {
            sf::Vector2f finalPos = rotatePoint(p, comp.rotation) + comp.position;
            sf::Color col = comp.isSelected ? sf::Color::Yellow : sf::Color::White;
            lines.append(sf::Vertex(finalPos, col));
        }

        window.draw(lines);
    }

    void drawAllComponents(sf::RenderWindow& window, const std::vector<Component>& list) {
        for (const auto& comp : list) {
            if (comp.type == RESISTOR) drawResistor(window, comp);
            else if (comp.type == CAPACITOR) drawCapacitor(window, comp);
            // Add LED logic later
        }
    }
} 