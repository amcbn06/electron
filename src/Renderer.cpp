#include "Renderer.hpp"
#include "Theme.hpp"
#include "Component.hpp"
#include "Constants.hpp"
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Renderer {

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

    void rotatePoint(sf::Vector2f origin, sf::Vector2f& point, float angleDegrees) {
        float rad = angleDegrees * M_PI / 180.0f;
        float s = std::sin(rad);
        float c = std::cos(rad);
        sf::Vector2f rel = point - origin;
        sf::Vector2f rotated(rel.x * c - rel.y * s, rel.x * s + rel.y * c);
        point = origin + rotated;
    }

    void drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b, sf::Color color = sf::Color::White) {
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(a, color);
        line[1] = sf::Vertex(b, color);
        window.draw(line);
    }

    void drawRectangle(sf::RenderWindow& window, sf::Vector2f upperLeft, sf::Vector2f lowerRight, sf::Color color = sf::Color::White) {
        sf::Vector2f upperRight{ lowerRight.x, upperLeft.y };
        sf::Vector2f lowerLeft{ upperLeft.x, lowerRight.y };

        sf::VertexArray lines(sf::Lines);
        lines.append(sf::Vertex(upperLeft,  color));
        lines.append(sf::Vertex(upperRight, color));

        lines.append(sf::Vertex(upperRight, color));
        lines.append(sf::Vertex(lowerRight, color));

        lines.append(sf::Vertex(lowerRight, color));
        lines.append(sf::Vertex(lowerLeft,  color));

        lines.append(sf::Vertex(lowerLeft,  color));
        lines.append(sf::Vertex(upperLeft,  color));

        window.draw(lines);
    }

    void drawEllipse(sf::RenderWindow& window, sf::Vector2f center, float a, float b, sf::Color color, bool full = false, int segments = 64) {
        if (segments < 3) segments = 3;

        if (full) {
            sf::ConvexShape poly;
            poly.setPointCount(segments);
            for (int i = 0; i < segments; ++i) {
                float t = (float)i / (float)segments * 2.0f * M_PI;
                sf::Vector2f p{a * std::cos(t), b * std::sin(t)};
                poly.setPoint(i, p);
            }
            poly.setPosition(center);
            poly.setFillColor(color);
            window.draw(poly);
        } else {
            sf::VertexArray strip(sf::LineStrip);
            strip.resize(segments + 1);
            for (int i = 0; i <= segments; ++i) {
                float t = (float)i / (float)segments * 2.0f * M_PI;
                sf::Vector2f p = center + sf::Vector2f(a * std::cos(t), b * std::sin(t));
                strip[i] = sf::Vertex(p, color);
            }
            window.draw(strip);
        }
    }
    
    void drawComponent(sf::RenderWindow& window, const Component& comp) {
        static sf::Font labelFont;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            labelFont.loadFromFile("assets/DejaVuSans.ttf");
            fontLoaded = true;
        }

        std::ifstream fin("assets/"+comp.type+".txt");
        std::string nume;
        std::getline(fin,nume);
        
        int cntleg;
        fin >> cntleg;

        for(int i= 0; i < cntleg; ++i){
            float x,y;
            fin >> x >> y;
            sf::Vector2f centru = comp.position + sf::Vector2f{x,y} * comp.scale;
            rotatePoint(comp.position, centru, comp.rotation);
            drawEllipse(window, centru, 3, 3, sf::Color::Red, true);
        }

        int cntpoints;
        fin >> cntpoints;
        std::vector<sf::Vector2f> rawPoints(cntpoints);
        for(int i= 0; i < cntpoints; ++i){
            char type;
            float a,b,c,d;
            fin>>type>>a>>b>>c>>d;
            sf::Color mycolor = comp.isSelected ? sf::Color::Yellow : sf::Color::White;
            if(type == 'L'){
                sf::Vector2f pos1 = comp.position + sf::Vector2f{a,b} * comp.scale;
                sf::Vector2f pos2 = comp.position + sf::Vector2f{c,d} * comp.scale;
                rotatePoint(comp.position,pos1, comp.rotation);
                rotatePoint(comp.position,pos2, comp.rotation);
                drawLine(window, pos1,pos2, mycolor);
            }
            if(type=='R'){
                sf::Vector2f pos1 = comp.position + sf::Vector2f{a,b} * comp.scale;
                sf::Vector2f pos2 = comp.position + sf::Vector2f{c,d} * comp.scale;
                rotatePoint(comp.position,pos1, comp.rotation);
                rotatePoint(comp.position,pos2, comp.rotation);
                drawRectangle(window, pos1,pos2, mycolor);
            }
            if(type=='O'){
                sf::Vector2f pos = comp.position + sf::Vector2f{a,b} * comp.scale;
                rotatePoint(comp.position,pos, comp.rotation);
                // std::cout<<comp.rotation<<'\n';
                if((int)comp.rotation / 90 % 2 == 1){
                    std::swap(c,d);
                }
                drawEllipse(window, pos, c * comp.scale, d  * comp.scale, mycolor);
            }
        }

        if (fontLoaded && !comp.simple) {
            std::ostringstream ss;
            if (!comp.unit.empty()) {
                ss << comp.unit << " ";
            }
            if(comp.value == INFINITY){
                ss << "?";
            }   else{
                ss << std::fixed << std::setprecision(2) << comp.value;
            }
            sf::Text label;
            label.setFont(labelFont);
            label.setString(ss.str());
            
            unsigned int charSize = Constants::text_size;
            label.setCharacterSize(charSize);
            label.setFillColor(sf::Color::White);

            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top);
            
            float textY = comp.position.y + 30.0f + (float)charSize;
            label.setPosition(comp.position.x, textY);
            window.draw(label);
        }
    }

    void drawMenu(sf::RenderWindow& window){
        sf::View prevView = window.getView();
        window.setView(window.getDefaultView());
        prevView = window.getView();
        window.setView(window.getDefaultView());
        // std::cerr << __LINE__ << std::endl;
        const float menuWidth = static_cast<float>(window.getSize().x) / 10.0f;
        sf::RectangleShape menuBg(sf::Vector2f(menuWidth, static_cast<float>(window.getSize().y)));
        menuBg.setPosition(0.f, 0.f);
        menuBg.setFillColor(sf::Color(24, 24, 24, 255));
        window.draw(menuBg);
        // std::cerr << __LINE__ << std::endl;

        std::vector<Component> palette;
        const float startX = menuWidth * 0.5f;
        const float startY = static_cast<float>(window.getSize().y) * 0.06f;
        const float bottomMargin = static_cast<float>(window.getSize().y) * 0.06f;
        float menuHeight = static_cast<float>(window.getSize().y);
        float spacing = (menuHeight - startY - bottomMargin) / static_cast<float>(types.size());
        // std::cerr << __LINE__ << std::endl;
        static std::vector<Component> menuComponents;
        if (menuComponents.empty()) {
            for (size_t i = 0; i < types.size(); ++i) {
                menuComponents.push_back(Component(
                    static_cast<int>(startX),
                    static_cast<int>(startY + static_cast<float>(i) * spacing),
                    types[i],
                    true,
                    0.5F
                ));
            }
        }
        for (size_t i = 0; i < types.size(); ++i) {
            Renderer::drawComponent(window, menuComponents[i]);
        }
        // std::cerr << __LINE__ << std::endl;

        window.setView(prevView);
        // std::cerr << "set view" << std::endl;
    }

    void drawAllComponents(sf::RenderWindow& window, const std::vector<Component>& list) {
        // std::cerr << "list.size() = " << list.size() << "\n";
        for (const auto& comp : list) {
            drawComponent(window, comp);
        }
        // std::cerr << "list.size() = " << list.size() << "\n";
    }
}