#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <chrono>

#include "Constants.hpp"
#include "Theme.hpp"
#include "Renderer.hpp"
#include "Component.hpp"

extern std::vector<Component> components;

// AppState
bool isPanning = false;
bool isEditing = false;
sf::Vector2i lastMousePixel;
std::string inputBuffer;
float zoomLevel = 1.0f;


void handleTextInput(sf::Event& event) {
    if (event.text.unicode == 8) { // backspace
        if (inputBuffer.size() > 0) {
            inputBuffer.pop_back();
        }
    }
    else if (event.text.unicode >= '0' && event.text.unicode <= '9') { // add digit
        inputBuffer += static_cast<char>(event.text.unicode);
    }
    else if (event.text.unicode == '.') { // add decimals
        if (inputBuffer.find('.') == std::string::npos) { // no dot already added
            inputBuffer += '.';
        }
    }

    int id = getSelection();
    if (inputBuffer.size() > 0) {
        components[id].value = std::stof(inputBuffer);
    }
    else {
        components[id].value = INFINITY;
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(1500, 1000), "Electron - Vizualizator de scheme electronice");
    window.setFramerateLimit(100);
    sf::View view = window.getDefaultView();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close event
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Keyboard events
            if (event.type == sf::Event::KeyPressed) {
                // Close (Ctrl + W)
                if (event.key.code == sf::Keyboard::W && event.key.control) {
                    window.close();
                }
                // Finish editing (Enter)
                if (event.key.code == sf::Keyboard::Enter) {
                    isEditing = false;
                }
                // Rotate (R)
                if (event.key.code == sf::Keyboard::R) {
                    int comp = getSelection();
                    if (comp != -1) {
                        rotate(components[comp]);
                    }
                }
                // Start editing (E)
                if (event.key.code == sf::Keyboard::E) {
                    int comp = getSelection();
                    if (comp != -1) {
                        isEditing = true;
                        inputBuffer.clear();
                    }
                }
            }

            // Text input (only if editing)
            if (event.type == sf::Event::TextEntered && isEditing) {
                handleTextInput(event);
            }

            // Mouse zoom
            if (event.type == sf::Event::MouseWheelScrolled) {
                int comp = getSelection();
                if (comp != -1) {
                    zoom(components[comp], event.mouseWheelScroll.delta > 0);
                }
                else {
                    float zoomChange = 1.0f;
                    if (event.mouseWheelScroll.delta > 0) {
                        zoomChange -= Constants::zoomSensitivity;
                    }
                    else {
                        zoomChange += Constants::zoomSensitivity;
                    }
                    // Clamp the zoom level
                    if (zoomLevel * zoomChange < Constants::zoomAlpha || zoomLevel * zoomChange > 1.0 / Constants::zoomAlpha) {
                        continue;
                    }
                    view.zoom(zoomChange);
                    zoomLevel *= zoomChange;
                }
            }

            // Mouse press
            if (event.type == sf::Event::MouseButtonPressed) {
                // Right Click -> Start panning
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = true;
                    lastMousePixel = sf::Mouse::getPosition(window);
                }
                // Left Click -> Select or Menu
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                    float menuWidth = static_cast<float>(window.getSize().x) / 10.0f;
                    float menuHeight = static_cast<float>(window.getSize().y);
                    float margin = menuHeight * 0.06f;
                    // Divide the space evenly for every component
                    float spacing = (menuHeight - 2 * margin) / static_cast<float>(types.size());

                    // in the Menu zone => select piece
                    if (mousePixel.x <= menuWidth) {
                        int index = static_cast<int>(std::round((mousePixel.y - margin) / spacing));
                        if (index >= 0 && index < (int)types.size()) {
                            for (auto& component : components) {
                                component.isSelected = false;
                            }
                            spawnComponent(types[index], window.getView().getCenter());
                            int newIndex = (int)components.size() - 1;
                            if (newIndex >= 0) {
                                components[newIndex].isSelected = true;
                            }
                            isEditing = false;
                            continue;
                        }
                    }

                    int selected = findClosest(window.mapPixelToCoords(mousePixel, view));
                    if (selected == -1) {
                        continue;
                    }
                    // Select the component under the cursor
                    if (not components[selected].isSelected) {
                        // Deselect everything else
                        for (auto& component : components) {
                            component.isSelected = false;
                        }
                        components[selected].isSelected = true;
                    }
                    // If too close to another component, don't deselect 
                    else if (not tooClose(window.mapPixelToCoords(mousePixel, view), selected)) {
                        components[selected].isSelected = false;
                        isEditing = false;
                    }
                }

            }

            // Mouse release
            if (event.type == sf::Event::MouseButtonReleased) {
                // Right Click Release -> Stop panning
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = false;
                }
                // Left Click Release -> Stop dragging
                // if (event.mouseButton.button == sf::Mouse::Left) {
                //     for (auto& component : components) {
                //         component.isSelected = false;
                //     }
                //     isEditing = false;
                // }
            }
        }

        // Handle panning
        if (isPanning) {
            sf::Vector2i currentMousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f lastWorld = window.mapPixelToCoords(lastMousePixel, view);
            sf::Vector2f currentWorld = window.mapPixelToCoords(currentMousePixel, view);
            sf::Vector2f delta = lastWorld - currentWorld;
            view.move(delta * Constants::panningSensitivity);
            lastMousePixel = currentMousePixel;
        }

        // Handle dragged components
        int selected = getSelection();
        if (selected != -1) {
            components[selected].position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
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