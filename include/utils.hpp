#pragma once
#include <SFML/Graphics.hpp>
#include "structs.hpp"

void roteste(float x, float y, float &xnou, float &ynou);
sf::Vector2f getAbsoluteNodePos(const piesa& P, int nodeIndex);
void drawSfmlArc(sf::RenderWindow& window, float x, float y, float radius, float startAngle, float endAngle, sf::Color color);
