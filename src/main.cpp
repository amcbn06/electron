#include <SFML/Graphics.hpp>
#include <cmath>
#include "Constants.hpp"
#include "Theme.hpp"
#include "Renderer.hpp"
#include <iostream>
#include <chrono>

// Global list of components
std::vector<Component> components;

long long getTime(){
    return std::chrono::steady_clock::now().time_since_epoch().count();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Electron - Vizualizator de scheme electronice");
    window.setFramerateLimit(60);

    // Camera State
    sf::View view = window.getDefaultView();

    float zoomLevel = 1.0f;
    bool isPanning = false;
    long double lastUpdated = getTime();

    // track mouse in pixel coordinates (integers) for mapPixelToCoords
    sf::Vector2i lastMousePixel;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                std::cerr << "Shutting down.\n";
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W && event.key.control) {
                std::cerr << "Ctrl + W pressed, shutting down.\n";
                window.close();
            }
            // Zooming logic (Middle Mouse Button Scroll)
            if (event.type == sf::Event::MouseWheelScrolled) {
                std::cerr << "Mouse Wheel Scrolled\n";
                float zoomChange = 1.0f;
                if (event.mouseWheelScroll.delta > 0) {
                    zoomChange -= Constants::zoomSensitivity;
                } else {
                    zoomChange += Constants::zoomSensitivity;
                }
                // Keep track of the current zooming degree, check if too big
                if(zoomLevel * zoomChange < Constants::zoomAlpha || zoomLevel * zoomChange > 1.0 / Constants::zoomAlpha) {
                    continue;
                }
                view.zoom(zoomChange);
                zoomLevel *= zoomChange;
            }

            // Panning Logic (Middle Mouse Button Click)
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                std::cerr << "Right Mouse Button Clicked, panning activated\n";
                isPanning = true;
                lastMousePixel = sf::Mouse::getPosition(window);
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                isPanning = false;
            }
            

            // Spawning Logic (Left Mouse Button Click)
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                std::cerr << "Left Mouse Button clicked, spawning RESISTOR\n";
                sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
                
                // Spawn a Resistor! (Hardcoded type for now)
                spawnComponent(components, "tranzistor_npn", mouseWorld);
            }
        }

        // Panning Logic
        if (isPanning && (getTime()-lastUpdated) > Constants::paddingDelay * 1e9) {
            sf::Vector2i currentMousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f lastWorld = window.mapPixelToCoords(lastMousePixel, view);
            sf::Vector2f currentWorld = window.mapPixelToCoords(currentMousePixel, view);
            sf::Vector2f delta = lastWorld - currentWorld;
            view.move(delta * Constants::panningSensitivity);
            lastMousePixel = currentMousePixel;
            lastUpdated = getTime();
        }

 
        // Clear the background
        window.clear(Theme::Background);
        window.setView(view);

        // 1. Draw Grid
        Renderer::drawGrid(window, view);

        // 2. Draw Components
        Renderer::drawAllComponents(window, components);
        
        window.display();

    }
    return 0;
}