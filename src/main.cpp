#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <string>
#include "../include/structs.hpp"
#include "../include/utils.hpp"
#include "../include/electron.hpp"

using namespace std;

float zoom = 7.0;
int gridSize = 14;

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Electron SFML - Proiect", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cout << "Warning: Font not found.\n";
    }

    piesa Meniu[MAX1];
    unsigned nrPieseMeniu = 12;
    const char* names[] = {"", "DIODA", "ZENNER", "TRANZNPN", "TRANZPNP", "CONDENS", 
                           "REZIST", "BATERIE", "POLARIZ", "SINU", "SERVOMOT", "AMPLOP", "STOP"};
    for (unsigned i = 1; i <= nrPieseMeniu; i++) {
        if (i <= 12) strcpy(Meniu[i].nume, names[i]);
        initializeaza(Meniu[i]);
    }

    std::vector<piesa> piesePeMasa;
    std::vector<Legatura> legaturi;
    int nextId = 1;
    
    int selectedPieceIdx = -1;
    bool isDragging = false;
    sf::Vector2f dragOffset;

    int connectNode1_PieceIdx = -1;
    int connectNode1_Index = -1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (my <= 60) {
                        int lat = window.getSize().x / nrPieseMeniu;
                        for (unsigned i = 1; i <= nrPieseMeniu; i++) {
                            if (abs(Meniu[i].x - mx) <= lat/2) {
                                if (strcmp(Meniu[i].nume, "STOP") == 0) {
                                    window.close();
                                } else {
                                    piesa noua = Meniu[i];
                                    noua.id = nextId++;
                                    noua.x = 600; 
                                    noua.y = 400;
                                    noua.orientare = 0;
                                    piesePeMasa.push_back(noua);
                                    selectedPieceIdx = piesePeMasa.size() - 1;
                                }
                            }
                        }
                    } 
                    else {
                        int pIdx, nIdx;
                        if (findNodeAt(piesePeMasa, mx, my, pIdx, nIdx)) {
                            if (connectNode1_PieceIdx == -1) {
                                connectNode1_PieceIdx = pIdx;
                                connectNode1_Index = nIdx;
                                cout << "Nod start selectat.\n";
                            } else {
                                if (connectNode1_PieceIdx != pIdx) {
                                    Legatura l;
                                    l.piesa1_id = piesePeMasa[connectNode1_PieceIdx].id;
                                    l.nod1_index = connectNode1_Index;
                                    l.piesa2_id = piesePeMasa[pIdx].id;
                                    l.nod2_index = nIdx;
                                    legaturi.push_back(l);
                                    cout << "Legatura creata.\n";
                                }
                                connectNode1_PieceIdx = -1;
                            }
                        } 
                        else {
                            int idx = findPieceAt(piesePeMasa, mx, my);
                            if (idx != -1) {
                                selectedPieceIdx = idx;
                                isDragging = true;
                                dragOffset.x = piesePeMasa[idx].x - mx;
                                dragOffset.y = piesePeMasa[idx].y - my;
                            } else {
                                selectedPieceIdx = -1;
                                connectNode1_PieceIdx = -1;
                            }
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                     int idx = findPieceAt(piesePeMasa, mx, my);
                     if(idx != -1) {
                         piesePeMasa[idx].orientare = (piesePeMasa[idx].orientare + 1) % 4;
                     }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                }
            }

            if (event.type == sf::Event::MouseMoved && isDragging && selectedPieceIdx != -1) {
                float rawX = event.mouseMove.x + dragOffset.x;
                float rawY = event.mouseMove.y + dragOffset.y;
                
                piesePeMasa[selectedPieceIdx].x = round(rawX / gridSize) * gridSize;
                piesePeMasa[selectedPieceIdx].y = round(rawY / gridSize) * gridSize;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Delete && selectedPieceIdx != -1) {
                    int idDel = piesePeMasa[selectedPieceIdx].id;
                    for(int k=legaturi.size()-1; k>=0; k--) {
                        if(legaturi[k].piesa1_id == idDel || legaturi[k].piesa2_id == idDel)
                            legaturi.erase(legaturi.begin() + k);
                    }
                    piesePeMasa.erase(piesePeMasa.begin() + selectedPieceIdx);
                    selectedPieceIdx = -1;
                }
                if (event.key.code == sf::Keyboard::E && selectedPieceIdx != -1) {
                    cout << "Introdu eticheta noua pt piesa (in consola): ";
                    cin >> piesePeMasa[selectedPieceIdx].eticheta;
                    cout << "Eticheta actualizata.\n";
                }
                if (event.key.code == sf::Keyboard::V) {
                    cout << "--- Verificare Circuit ---\n";
                    if (piesePeMasa.empty()) cout << "Circuit gol.\n";
                    else {
                        int noduriConectate = 0;
                        int totalNoduri = 0;
                        for(const auto& p : piesePeMasa) totalNoduri += p.nrLegaturi;
                        
                        cout << "Total piese: " << piesePeMasa.size() << "\n";
                        cout << "Total legaturi: " << legaturi.size() << "\n";
                    }
                    cout << "--------------------------\n";
                }
            }
        }

        window.clear(sf::Color::Black);

        sf::RectangleShape dot(sf::Vector2f(2, 2));
        dot.setFillColor(sf::Color(40, 40, 40));
        for(int y=0; y<window.getSize().y; y+=gridSize) {
            for(int x=0; x<window.getSize().x; x+=gridSize) {
                dot.setPosition(x, y);
                window.draw(dot);
            }
        }

        deseneazaMeniul(window, Meniu, nrPieseMeniu, font);

        for(const auto& l : legaturi) {
            int idx1 = findPieceIndexById(piesePeMasa, l.piesa1_id);
            int idx2 = findPieceIndexById(piesePeMasa, l.piesa2_id);
            
            if(idx1 != -1 && idx2 != -1) {
                sf::Vector2f p1 = getAbsoluteNodePos(piesePeMasa[idx1], l.nod1_index);
                sf::Vector2f p2 = getAbsoluteNodePos(piesePeMasa[idx2], l.nod2_index);
                
                sf::Vertex line[] = {
                    sf::Vertex(p1, sf::Color::Yellow),
                    sf::Vertex(p2, sf::Color::Yellow)
                };
                window.draw(line, 2, sf::Lines);
            }
        }

        if (connectNode1_PieceIdx != -1) {
             sf::Vector2f p1 = getAbsoluteNodePos(piesePeMasa[connectNode1_PieceIdx], connectNode1_Index);
             sf::Vector2f pMouse((float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y);
             sf::Vertex line[] = {
                sf::Vertex(p1, sf::Color::Cyan),
                sf::Vertex(pMouse, sf::Color::Cyan)
            };
            window.draw(line, 2, sf::Lines);
        }

        for (int i=0; i<piesePeMasa.size(); i++) {
            sf::Color c = (i == selectedPieceIdx) ? sf::Color::Cyan : sf::Color::Green;
            deseneaza(window, piesePeMasa[i], c);
            
            sf::Text lbl;
            lbl.setFont(font);
            lbl.setString(piesePeMasa[i].eticheta);
            lbl.setCharacterSize(12);
            lbl.setFillColor(sf::Color::White);
            lbl.setPosition(piesePeMasa[i].x - 10, piesePeMasa[i].y + 20);
            window.draw(lbl);
        }

        sf::Text help;
        help.setFont(font);
        help.setString("Click: Select/Move | Right Click: Rotate | Click Nodes: Connect | E: Edit Label | Del: Delete");
        help.setCharacterSize(14);
        help.setFillColor(sf::Color(150, 150, 150));
        help.setPosition(10, window.getSize().y - 20);
        window.draw(help);

        window.display();
    }

    return 0;
}
