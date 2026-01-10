#include <SFML/Graphics.hpp>

#include <chrono>
#include <cmath>
#include <iostream>

#include "Component.hpp"
#include "Constants.hpp"
#include "Renderer.hpp"
#include "SaveManager.hpp"
#include "Theme.hpp"


extern std::vector<Component> components;
extern std::vector<Wire> wires;


bool isPanning = false;
bool isEditing = false;
sf::Vector2i lastMousePixel;
std::string inputBuffer;
float zoomLevel = 1.0f;


bool isWiring = false;
std::pair<int, int> pendingPin(-1, -1);


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
    if (event.text.unicode == 8) { 
        if (inputBuffer.size() > 0) {
            inputBuffer.pop_back();
        }
    }
    else if (event.text.unicode >= '0' && event.text.unicode <= '9') { 
        inputBuffer += static_cast<char>(event.text.unicode);
    }
    else if (event.text.unicode == '.') { 
        if (inputBuffer.find('.') == std::string::npos) { 
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
    sf::RenderWindow window(sf::VideoMode(1300, 720), "Electron - Vizualizator de scheme electronice");
    window.setFramerateLimit(100);
    sf::View view = window.getDefaultView();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                
                if (event.key.code == sf::Keyboard::W && event.key.control) {
                    window.close();
                }
                
                if (event.key.code == sf::Keyboard::Escape) {
                    stopAnyAction();
                }
                
                if (event.key.code == sf::Keyboard::R) {
                    int comp = getSelection();
                    if (comp != -1) {
                        rotate(components[comp]);
                    }
                }

                if(event.key.code == sf::Keyboard::Delete){
                    int comp = getSelection();
                    if(comp != -1){
                        components[comp].sters = true;
                        components[comp].isSelected = false;
                    }
                }
                
                if (event.key.code == sf::Keyboard::E) {
                    int comp = getSelection();
                    if (comp != -1) {
                        isEditing = true;
                        inputBuffer.clear();
                    }
                }

                
                if (event.key.code == sf::Keyboard::S && event.key.control) {
                    stopAnyAction();
                    
                    
                    std::cout << "\n[System] Enter filename to SAVE (e.g. 'my_circuit'): ";
                    std::string filename;
                    std::getline(std::cin, filename);
                    
                    
                    if (!filename.empty()) {
                        
                        if (filename.find(".txt") == std::string::npos) {
                            filename += ".txt";
                        }
                        SaveManager::saveCircuit(filename, components, wires);
                    }
                }

                
                if (event.key.code == sf::Keyboard::O && event.key.control) {
                    stopAnyAction();

                    std::cout << "\n[System] Enter filename to LOAD (e.g. 'my_circuit'): ";
                    std::string filename;
                    std::getline(std::cin, filename);
                    
                    if (!filename.empty()) {
                        if (filename.find(".txt") == std::string::npos) {
                            filename += ".txt";
                        }
                        SaveManager::loadCircuit(filename, components, wires);
                    }
                }
            }

            
            if (event.type == sf::Event::TextEntered && isEditing) {
                handleTextInput(event);
            }

            
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
                    
                    if (zoomLevel * zoomChange < Constants::zoomAlpha || zoomLevel * zoomChange > 1.0 / Constants::zoomAlpha) {
                        continue;
                    }
                    view.zoom(zoomChange);
                    zoomLevel *= zoomChange;
                }
            }

            
            if (event.type == sf::Event::MouseButtonPressed) {
                
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = true;
                    lastMousePixel = sf::Mouse::getPosition(window);
                }
                
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel, view);
                    
                    float menuWidth = static_cast<float>(window.getSize().x) / 10.0f;
                    
                    if (mousePixel.x <= menuWidth) {
                        if (getSelection() != -1) {
                            stopSelecting();
                            continue;
                        }
                        float menuHeight = static_cast<float>(window.getSize().y);
                        float margin = menuHeight * 0.06f;
                        
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

                    
                    std::pair<int, int> clickedPin = findPinAt(mouseWorld);

                    if (clickedPin.first != -1) {
                        
                        if (isWiring == false) {
                            isWiring = true;
                            pendingPin = clickedPin;
                            stopSelecting();
                        }
                        else {
                            
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
                        
                        
                        stopWiring();
                        continue;
                    }

                    int selected = findClosest(mouseWorld);
                    if (selected == -1) {
                        stopSelecting();
                        continue;
                    }
                    
                    if (components[selected].isSelected == false) {
                        stopSelecting();
                        components[selected].isSelected = true;
                    }
                    
                    else if (tooClose(window.mapPixelToCoords(mousePixel, view), selected) == false) {
                        stopSelecting();
                    }
                }
            }

            
            if (event.type == sf::Event::MouseButtonReleased) {
                
                if (event.mouseButton.button == sf::Mouse::Right) {
                    isPanning = false;
                }
            }
        }

        
        if (isPanning) {
            sf::Vector2i currentMousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f lastWorld = window.mapPixelToCoords(lastMousePixel, view);
            sf::Vector2f currentWorld = window.mapPixelToCoords(currentMousePixel, view);
            sf::Vector2f delta = lastWorld - currentWorld;
            view.move(delta * Constants::panningSensitivity);
            lastMousePixel = currentMousePixel;
        }

        
        int selected = getSelection();
        if (selected != -1) {
            components[selected].position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }

        window.clear(Theme::Background);

        Renderer::drawGrid(window, view);

        
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