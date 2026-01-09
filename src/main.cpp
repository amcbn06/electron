#include <SFML/Graphics.hpp>

#include <chrono>
#include <cmath>
#include <iostream>

#include "Component.hpp"
#include "Constants.hpp"
#include "Renderer.hpp"
#include "Theme.hpp"

// Reference the global components and wires
extern std::vector<Component> components;
extern std::vector<Wire> wires;

// App State
bool isPanning = false;
bool isEditing = false;
sf::Vector2i lastMousePixel;
std::string inputBuffer;
float zoomLevel = 1.0f;

// Wiring State
bool isWiring = false;
std::pair<int, int> pendingPin(-1, -1);

// Helper methods to avoid logic colisions
void stopEditing() {
    isEditing = false;
    inputBuffer.clear();
}

void stopSelecting() {
    for (auto& component : components) {
        component.isSelected = false;
    }
    stopEditing();
}

void stopWiring() {
    isWiring = false;
    pendingPin = std::make_pair(-1, -1);
}

void stopAnyAction() {
    stopWiring();
    stopSelecting();
}

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
                // Stop any action we are doing (Esc)
                if (event.key.code == sf::Keyboard::Escape) {
                    stopAnyAction();
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
                // Right Click -> Start panning / stop wiring
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = true;
                    lastMousePixel = sf::Mouse::getPosition(window);
                }
                // Left Click -> Select or Menu
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel, view);
                    
                    float menuWidth = static_cast<float>(window.getSize().x) / 10.0f;
                    // In the Menu zone => Select piece
                    if (mousePixel.x <= menuWidth) {
                        if (getSelection() != -1) {
                            stopSelecting();
                            continue;
                        }
                        float menuHeight = static_cast<float>(window.getSize().y);
                        float margin = menuHeight * 0.06f;
                        // Divide the space evenly for every component
                        float spacing = (menuHeight - 2 * margin) / static_cast<float>(types.size());

                        int optionIndex = static_cast<int>(std::round((mousePixel.y - margin) / spacing));
                        if (optionIndex >= 0 && optionIndex < (int)types.size()) {
                            stopSelecting();
                            spawnComponent(types[optionIndex], window.getView().getCenter());
                            int index = (int)components.size() - 1;
                            components[index].isSelected = true;
                        }
                        continue;
                    }

                    // Check for pin selection first
                    std::pair<int, int> clickedPin = findPinAt(mouseWorld);

                    if (clickedPin.first != -1) {
                        // Start wiring
                        if (isWiring == false) {
                            isWiring = true;
                            pendingPin = clickedPin;
                            stopSelecting();
                        }
                        else {
                            // if the pins are from different components, wire them
                            if (pendingPin.first != clickedPin.first) {
                                wires.push_back(Wire{
                                    pendingPin.first,
                                    pendingPin.second,
                                    clickedPin.first,
                                    clickedPin.second
                                });
                            }
                            stopWiring();
                        }
                        continue;
                    }
                    else if (isWiring) {
                        // if we clicked anything other than a pin
                        // while wiring, cancel wiring
                        stopWiring();
                        continue;
                    }

                    int selected = findClosest(mouseWorld);
                    if (selected == -1) {
                        stopSelecting();
                        continue;
                    }
                    // Select the component under the cursor
                    if (components[selected].isSelected == false) {
                        stopSelecting();
                        components[selected].isSelected = true;
                    }
                    // If too close to another component, don't deselect 
                    else if (tooClose(window.mapPixelToCoords(mousePixel, view), selected) == false) {
                        stopSelecting();
                    }
                }
            }

            // Mouse release
            if (event.type == sf::Event::MouseButtonReleased) {
                // Right Click Release -> Stop panning
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = false;
                }
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

        // if wiring and got a valid start pin, draw a ghost wire
        if (isWiring && pendingPin.first != -1) {
            auto& startComponent = components[pendingPin.first];
            sf::Vector2f startPin = startComponent.getAbsPin(pendingPin.second);
            
            float startThresh = startComponent.scale * 1.5f;

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
            sf::Vector2f mouseCenter(-9999, -9999); 

            Renderer::drawAutoRoute(window, startPin, mousePos, 
                                    startComponent.position, startThresh, 
                                    mouseCenter, 0.0f, 
                                    Theme::Wire::ghost);

        }

        Renderer::drawAllComponents(window);

        Renderer::drawMenu(window);

        window.setView(view);
        window.display();
    }
    return 0;
}