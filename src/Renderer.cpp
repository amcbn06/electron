#include "Renderer.hpp"
#include "Theme.hpp"
#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>
#include <string>
#include <fstream>

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


    void drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b) {
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(a, sf::Color::White);
        line[1] = sf::Vertex(b, sf::Color::White);
        window.draw(line);
    }

    void drawRectangle(sf::RenderWindow& window, sf::Vector2f upperLeft, sf::Vector2f lowerRight) {
        sf::Vector2f upperRight{ lowerRight.x, upperLeft.y };
        sf::Vector2f lowerLeft{ upperLeft.x, lowerRight.y };

        sf::VertexArray lines(sf::Lines);
        lines.append(sf::Vertex(upperLeft,  sf::Color::White));
        lines.append(sf::Vertex(upperRight, sf::Color::White));

        lines.append(sf::Vertex(upperRight, sf::Color::White));
        lines.append(sf::Vertex(lowerRight, sf::Color::White));

        lines.append(sf::Vertex(lowerRight, sf::Color::White));
        lines.append(sf::Vertex(lowerLeft,  sf::Color::White));

        lines.append(sf::Vertex(lowerLeft,  sf::Color::White));
        lines.append(sf::Vertex(upperLeft,  sf::Color::White));

        window.draw(lines);
    }

    void drawEllipse(sf::RenderWindow& window, sf::Vector2f center, float a, float b, int segments = 64) {
        sf::VertexArray strip(sf::LineStrip);
        strip.resize(segments + 1);
        for (int i = 0; i <= segments; ++i) {
            float t = (float)i / (float)segments * 2.0f * M_PI;
            sf::Vector2f p = center + sf::Vector2f(a * std::cos(t), b * std::sin(t));
            strip[i] = sf::Vertex(p, sf::Color::White);
        }
        window.draw(strip);
    }
    


    void drawComponent(sf::RenderWindow& window, const Component& comp)
    {
        std::ifstream fin("assets/"+comp.type+".txt");
        std::string nume;
        std::getline(fin,nume);
        
        int cntleg;
        fin>>cntleg;

        for(int i= 0; i < cntleg; ++i){
            float x,y;
            fin>>x>>y;
            sf::Vector2f centru = comp.position + sf::Vector2f{x,y};
            drawEllipse(window, centru, 0.2 , 0.2);
        }

        int cntpoints;
        fin>>cntpoints;
        std::vector<sf::Vector2f> rawPoints(cntpoints);
        for(int i= 0; i < cntpoints; ++i){
            char type;
            float a,b,c,d;
            fin>>type>>a>>b>>c>>d;
            if(type == 'L'){
                sf::Vector2f pos1 = comp.position + sf::Vector2f{a,b} * Constants::pb_scale;
                sf::Vector2f pos2 = comp.position + sf::Vector2{c,d} * Constants::pb_scale;
                drawLine(window, pos1,pos2);
            }
        }



    }

    void drawResistor(sf::RenderWindow& window, const Component& comp) {
        sf::VertexArray lines(sf::Lines);
        std::vector<sf::Vector2f> rawPoints = {
            {-2.25f, -2.25f}, {-3.0f, -2.25f},
            {-2.25f,  0.75f}, {-3.0f,  0.75f},
            {-2.625f, 0.375f}, {-2.625f, 1.125f},
            {-1.5f,  -3.0f},  {-1.5f,  3.0f},
            {-1.5f,  -3.0f},  {1.5f,   0.0f},
            {-1.5f,   3.0f},  {1.5f,   0.0f},
            {1.5f,    0.0f},  {4.5f,   0.0f},
            {-1.5f,  -1.5f},  {-4.5f, -1.5f},
            {-1.5f,   1.5f},  {-4.5f,  1.5f}
        };

        for (auto& p : rawPoints) {
            p.x *= 10;
            p.y *= 10;
            sf::Vector2f finalPos = rotatePoint(p, comp.rotation) + comp.position;
            sf::Color col = comp.isSelected ? sf::Color::Yellow : sf::Color::White;
            lines.append(sf::Vertex(finalPos, col));
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
            drawComponent(window, comp);
        }
    }
} 