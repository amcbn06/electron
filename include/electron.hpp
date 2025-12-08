#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "structs.hpp"

void initializeaza(piesa& P);
void deseneaza(sf::RenderWindow& window, piesa P, sf::Color color);
void deseneazaMeniul(sf::RenderWindow& window, piesa Meniu[], unsigned nrPieseMeniu, sf::Font& font);
int findPieceAt(const std::vector<piesa>& piese, int mx, int my);
bool findNodeAt(const std::vector<piesa>& piese, int mx, int my, int& outPiesaIdx, int& outNodIdx);
int findPieceIndexById(const std::vector<piesa>& piese, int id);
