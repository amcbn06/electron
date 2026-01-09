#include "Renderer.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "Component.hpp"
#include "Constants.hpp"
#include "Theme.hpp"
#include "Utils.hpp"

namespace Renderer {
    // Fundamental shapes
    void drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b, sf::Color color) {
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(a, color);
        line[1] = sf::Vertex(b, color);
        window.draw(line);
    }

    void drawRectangle(sf::RenderWindow& window, sf::Vector2f upperLeft, sf::Vector2f lowerRight, sf::Color color) {
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

    void drawEllipse(sf::RenderWindow& window, sf::Vector2f center, float a, float b, sf::Color color, bool full, int segments) {
        if (segments < 3) segments = 3;

        if (full) {
            sf::ConvexShape poly;
            poly.setPointCount(segments);
            for (int i = 0; i < segments; ++i) {
                float t = (float)i / (float)segments * 2.0f * Constants::PI;
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
                float t = (float)i / (float)segments * 2.0f * Constants::PI;
                sf::Vector2f p = center + sf::Vector2f(a * std::cos(t), b * std::sin(t));
                strip[i] = sf::Vertex(p, color);
            }
            window.draw(strip);
        }
    }

    // Project related
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

    bool isBlocked(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f compPos, float threshold) {
        // !!! threshold should be calculated according to scale
        // vertical wire
        if (std::abs(p1.x - p2.x) < 1.0f) { 
            // (component X close to wire X) AND (component Y between wire start/end)
            bool xMatch = std::abs(compPos.x - p1.x) < threshold;
            bool yBetween = compPos.y > std::min(p1.y, p2.y) && compPos.y < std::max(p1.y, p2.y);
            return xMatch && yBetween;
        } 
        // horizontal wire
        else {
            // (component Y close to wire Y) AND (component X between wire start/end)
            bool yMatch = std::abs(compPos.y - p1.y) < threshold;
            bool xBetween = compPos.x > std::min(p1.x, p2.x) && compPos.x < std::max(p1.x, p2.x);
            return yMatch && xBetween;
        }
    }

    bool checkPath(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f startComp, float startSize, sf::Vector2f endComp, float endSize) {
        // check both the start and end component
        return isBlocked(p1, p2, startComp, startSize) || isBlocked(p1, p2, endComp, endSize);
    }

    void drawAutoRoute(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end,  sf::Vector2f startComp, float startSize, sf::Vector2f endComp, float endSize, sf::Color color) {
        float deltaX = std::abs(end.x - start.x);
        float deltaY = std::abs(end.y - start.y);

        // !!! detour should be calculated according to scale
        float detour = std::max(startSize, endSize) * 2.0f;

        // farther horizontally: try S (H-V-H)
        if (deltaX > deltaY) {
            float midX = (start.x + end.x) / 2.0f;
            sf::Vector2f c1(midX, start.y);
            sf::Vector2f c2(midX, end.y);

            // do any of the segments hit the components?
            bool collision = checkPath(start, c1, startComp, startSize, endComp, endSize) ||
                        checkPath(c1, c2, startComp, startSize, endComp, endSize) ||
                        checkPath(c2, end, startComp, startSize, endComp, endSize);

            if (collision == false) {
                drawLine(window, start, c1, color);
                drawLine(window, c1, c2, color);
                drawLine(window, c2, end, color);
            } else {
                // switch to vertical C shape
                float detourY = std::min(start.y, end.y) - detour; // try UP
                sf::Vector2f d1(start.x, detourY);
                sf::Vector2f d2(end.x, detourY);

                // if UP crashes, go DOWN
                if (checkPath(start, d1, startComp, startSize, endComp, endSize) || checkPath(d1, d2, startComp, startSize, endComp, endSize)) {
                    detourY = std::max(start.y, end.y) + detour; 
                    d1 = sf::Vector2f(start.x, detourY);
                    d2 = sf::Vector2f(end.x, detourY);
                }

                drawLine(window, start, d1, color);
                drawLine(window, d1, d2, color);
                drawLine(window, d2, end, color);
            }
        }
        // farther vertically: try S (V-H-V)
        else {
            float midY = (start.y + end.y) / 2.0f;
            sf::Vector2f c1(start.x, midY);
            sf::Vector2f c2(end.x, midY);

            bool collision = checkPath(start, c1, startComp, startSize, endComp, endSize) ||
                        checkPath(c1, c2, startComp, startSize, endComp, endSize) ||
                        checkPath(c2, end, startComp, startSize, endComp, endSize);

            if (collision == false) {
                drawLine(window, start, c1, color);
                drawLine(window, c1, c2, color);
                drawLine(window, c2, end, color);
            } else {
                // switch to horizontal C shape
                float detourX = std::min(start.x, end.x) - detour; // try LEFT
                sf::Vector2f d1(detourX, start.y);
                sf::Vector2f d2(detourX, end.y);

                // if LEFT crashes, go RIGHT
                if (checkPath(start, d1, startComp, startSize, endComp, endSize) || checkPath(d1, d2, startComp, startSize, endComp, endSize)) {
                    detourX = std::max(start.x, end.x) + detour;
                    d1 = sf::Vector2f(detourX, start.y);
                    d2 = sf::Vector2f(detourX, end.y);
                }

                drawLine(window, d1, start, color);
                drawLine(window, d1, d2, color);
                drawLine(window, d2, end, color);
            }
        }
    }

    void drawWires(sf::RenderWindow& window) {
        // lambda function to ensure valid indicies
        auto check = [&](int index)->bool {
            return index >= 0 && index < components.size();
        };

        for (const auto& wire : wires) {
            if (check(wire.startComponentIndex) && check(wire.endComponentIndex)) {
                const auto& startComponent = components[wire.startComponentIndex];
                const auto& endComponent = components[wire.endComponentIndex];

                sf::Vector2f startPin = startComponent.getAbsPin(wire.startPinIndex);
                sf::Vector2f endPin = endComponent.getAbsPin(wire.endPinIndex);

                // scale + 50% margin
                float startThresh = startComponent.scale * 1.5f; 
                float endThresh = endComponent.scale * 1.5f;

                drawAutoRoute(window, startPin, endPin,
                    startComponent.position, startThresh, 
                    endComponent.position, endThresh, 
                    Theme::Wire::idle);
            }
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
            Utils::rotatePoint(comp.position, centru, comp.rotation);
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
                Utils::rotatePoint(comp.position,pos1, comp.rotation);
                Utils::rotatePoint(comp.position,pos2, comp.rotation);
                drawLine(window, pos1,pos2, mycolor);
            }
            if(type=='R'){
                sf::Vector2f pos1 = comp.position + sf::Vector2f{a,b} * comp.scale;
                sf::Vector2f pos2 = comp.position + sf::Vector2f{c,d} * comp.scale;
                Utils::rotatePoint(comp.position,pos1, comp.rotation);
                Utils::rotatePoint(comp.position,pos2, comp.rotation);
                drawRectangle(window, pos1,pos2, mycolor);
            }
            if(type=='O'){
                sf::Vector2f pos = comp.position + sf::Vector2f{a,b} * comp.scale;
                Utils::rotatePoint(comp.position,pos, comp.rotation);
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

    void drawAllComponents(sf::RenderWindow& window) {
        // draw wires behind components
        drawWires(window);

        for (const auto& comp : components) {
            drawComponent(window, comp);
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
}