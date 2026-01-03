#include <SFML/Graphics.hpp>
#include <cmath>
#include "Constants.hpp"
#include "Theme.hpp"
#include "Renderer.hpp"
#include <iostream>
#include <chrono>

extern std::vector<Component> components;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1500, 1000), "Electron - Vizualizator de scheme electronice");
    window.setFramerateLimit(100);

    sf::View view = window.getDefaultView();

    float zoomLevel = 1.0f;
    bool isPanning = false;

    bool isEditing = false;

    sf::Vector2i lastMousePixel;

    std::string inputBuffer;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W && event.key.control)
                {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Enter)
                {
                    isEditing = false;
                }

                if (event.key.code == sf::Keyboard::R)
                {
                    int comp = get_selection();
                    if (comp != -1)
                    {
                        rotate(components[comp]);
                    }
                }

                if (event.key.code == sf::Keyboard::E)
                {
                    int comp = get_selection();
                    if (comp != -1)
                    {
                        isEditing = true;
                        inputBuffer.clear();
                    }
                }
            }

            if (event.type == sf::Event::TextEntered && isEditing)
            {

                if (event.text.unicode == 8)
                {
                    if (!inputBuffer.empty())
                    {
                        inputBuffer.pop_back();
                    }
                }

                else if (event.text.unicode >= '0' && event.text.unicode <= '9')
                {
                    inputBuffer += static_cast<char>(event.text.unicode);
                }

                else if (event.text.unicode == '.')
                {
                    if (inputBuffer.find('.') == std::string::npos)
                    {
                        inputBuffer += '.';
                    }
                }

                int comp = get_selection();
                if (!inputBuffer.empty())
                {
                    components[comp].valoare = std::stof(inputBuffer);
                }
                else
                {
                    components[comp].valoare = INFINITY;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled)
            {

                int comp = get_selection();
                if (comp != -1)
                {
                    zoom(components[comp], event.mouseWheelScroll.delta > 0);
                    continue;
                }

                float zoomChange = 1.0f;
                if (event.mouseWheelScroll.delta > 0)
                {
                    zoomChange -= Constants::zoomSensitivity;
                }
                else
                {
                    zoomChange += Constants::zoomSensitivity;
                }
                if (zoomLevel * zoomChange < Constants::zoomAlpha || zoomLevel * zoomChange > 1.0 / Constants::zoomAlpha)
                {
                    continue;
                }
                view.zoom(zoomChange);
                zoomLevel *= zoomChange;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                isPanning = true;
                lastMousePixel = sf::Mouse::getPosition(window);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                float menuWidth = static_cast<float>(window.getSize().x) / 10.0f;
                float menuHeight = static_cast<float>(window.getSize().y);
                float startY = menuHeight * 0.06f;
                float bottomMargin = menuHeight * 0.06f;
                float spacing = (menuHeight - startY - bottomMargin) / static_cast<float>(types.size());

                if (mousePixel.x <= (int)menuWidth)
                {
                    int idx = static_cast<int>(std::round((mousePixel.y - startY) / spacing));
                    if (idx >= 0 && idx < (int)types.size())
                    {
                        for (auto &p : components) p.isSelected = false;
                        spawnComponent(types[idx], window.getView().getCenter());
                        int newIndex = static_cast<int>(components.size()) - 1;
                        if (newIndex >= 0) components[newIndex].isSelected = true;
                        isEditing = false;
                        continue;
                    }
                }

                int comp = find_closest(window.mapPixelToCoords(mousePixel, view));
                if (comp != -1)
                {
                    if (!components[comp].isSelected)
                    {
                        for (auto &p : components)
                        {
                            p.isSelected = false;
                        }
                        components[comp].isSelected = true;
                    }
                    else
                    {
                        if (!too_close(window.mapPixelToCoords(mousePixel, view), comp))
                        {
                            components[comp].isSelected = false;
                            isEditing = false;
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
            {
                isPanning = false;
            }
        }

        if (isPanning)
        {
            sf::Vector2i currentMousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f lastWorld = window.mapPixelToCoords(lastMousePixel, view);
            sf::Vector2f currentWorld = window.mapPixelToCoords(currentMousePixel, view);
            sf::Vector2f delta = lastWorld - currentWorld;
            view.move(delta * Constants::panningSensitivity);
            lastMousePixel = currentMousePixel;
        }

        if (get_selection() != -1)
        {
            int c = get_selection();
            components[c].position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }

        window.clear(Theme::Background);

        Renderer::drawGrid(window, view);

        Renderer::drawAllComponents(window, components);

        Renderer::drawMenu(window);

        window.setView(view);

        window.display();
    }
    return 0;
}