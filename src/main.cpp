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

    saveFile currentSave;
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
                        if (lat > 0) {
                            int clicked = mx / lat + 1;
                            if (clicked < 1) clicked = 1;
                            if (clicked > (int)nrPieseMeniu) clicked = nrPieseMeniu;
                            int centerX = lat * clicked - lat / 2;
                            if (abs(centerX - mx) <= lat / 2) {
                                if (strcmp(Meniu[clicked].nume, "STOP") == 0) {
                                    window.close();
                                } else {
                                    piesa noua = Meniu[clicked];
                                    noua.id = nextId++;
                                    noua.x = 600;
                                    noua.y = 400;
                                    noua.orientare = 0;
                                    currentSave.piese.push_back(noua);
                                    selectedPieceIdx = currentSave.piese.size() - 1;
                                }
                            }
                        }
                    } 
                    else {
                        int pIdx, nIdx;
                        if (findNodeAt(currentSave.piese, mx, my, pIdx, nIdx)) {
                            if (connectNode1_PieceIdx == -1) {
                                connectNode1_PieceIdx = pIdx;
                                connectNode1_Index = nIdx;
                                cout << "Nod start selectat.\n";
                            } else {
                                if (connectNode1_PieceIdx != pIdx) {
                                    Legatura l;
                                    l.piesa1_id = currentSave.piese[connectNode1_PieceIdx].id;
                                    l.nod1_index = connectNode1_Index;
                                    l.piesa2_id = currentSave.piese[pIdx].id;
                                    l.nod2_index = nIdx;
                                    currentSave.legaturi.push_back(l);
                                    cout << "Legatura creata.\n";
                                }
                                connectNode1_PieceIdx = -1;
                            }
                        } 
                        else {
                            int idx = findPieceAt(currentSave.piese, mx, my);
                            if (idx != -1) {
                                selectedPieceIdx = idx;
                                isDragging = true;
                                dragOffset.x = currentSave.piese[idx].x - mx;
                                dragOffset.y = currentSave.piese[idx].y - my;
                            } else {
                                selectedPieceIdx = -1;
                                connectNode1_PieceIdx = -1;
                            }
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                     int idx = findPieceAt(currentSave.piese, mx, my);
                     if(idx != -1) {
                         currentSave.piese[idx].orientare = (currentSave.piese[idx].orientare + 1) % 4;
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

                currentSave.piese[selectedPieceIdx].x = round(rawX / gridSize) * gridSize;
                currentSave.piese[selectedPieceIdx].y = round(rawY / gridSize) * gridSize;
            }

            if(event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    cout<<"Introdu numarul salvarii (ex: 1): ";
                    int saveId;
                    cin >> saveId;
                    save(saveId, currentSave);
                    cout << "Circuit salvat.\n";
                }
                if (event.key.code == sf::Keyboard::L) {
                    cout<<"Introdu numarul salvarii de incarcat (ex: 1): ";
                    int loadId;
                    cin >> loadId;
                    currentSave = load(loadId);
                    nextId = 1;
                    for (const auto& p : currentSave.piese) {
                        if (p.id >= nextId) nextId = p.id + 1;
                    }
                    for(auto& p : currentSave.piese){
                        initializeaza(p);
                    }
                    selectedPieceIdx = -1;
                    connectNode1_PieceIdx = -1;
                    cout << "Circuit incarcat.\n";
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Delete && selectedPieceIdx != -1) {
                    int idDel = currentSave.piese[selectedPieceIdx].id;
                    for(int k=currentSave.legaturi.size()-1; k>=0; k--) {
                        if(currentSave.legaturi[k].piesa1_id == idDel || currentSave.legaturi[k].piesa2_id == idDel)
                            currentSave.legaturi.erase(currentSave.legaturi.begin() + k);
                    }
                    currentSave.piese.erase(currentSave.piese.begin() + selectedPieceIdx);
                    selectedPieceIdx = -1;
                }
                if (event.key.code == sf::Keyboard::E && selectedPieceIdx != -1) {
                    cout << "Introdu eticheta noua pt piesa (in consola): ";
                    cin.getline(currentSave.piese[selectedPieceIdx].eticheta, 100);
                    cout << "Eticheta actualizata.\n";
                }
                if (event.key.code == sf::Keyboard::V) {
                    cout << "--- Verificare Circuit ---\n";
                    if (currentSave.piese.empty()) cout << "Circuit gol.\n";
                    else {
                        int noduriConectate = 0;
                        int totalNoduri = 0;
                        for(const auto& p : currentSave.piese) totalNoduri += p.nrLegaturi;

                        cout << "Total piese: " << currentSave.piese.size() << "\n";
                        cout << "Total legaturi: " << currentSave.legaturi.size() << "\n";
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

        for(const auto& l : currentSave.legaturi) {
            int idx1 = findPieceIndexById(currentSave.piese, l.piesa1_id);
            int idx2 = findPieceIndexById(currentSave.piese, l.piesa2_id);

            if(idx1 != -1 && idx2 != -1) {
                sf::Vector2f p1 = getAbsoluteNodePos(currentSave.piese[idx1], l.nod1_index);
                sf::Vector2f p2 = getAbsoluteNodePos(currentSave.piese[idx2], l.nod2_index);

                auto drawManhattan = [&](const sf::Vector2f& a, const sf::Vector2f& b, sf::Color col) {
                    if (std::abs(a.x - b.x) < 1.0f || std::abs(a.y - b.y) < 1.0f) {
                        sf::Vertex line[] = { sf::Vertex(a, col), sf::Vertex(b, col) };
                        window.draw(line, 2, sf::Lines);
                        return;
                    }

                    sf::Vector2f corner(a.x, b.y);
                    if ((corner == a) || (corner == b)) corner = sf::Vector2f(b.x, a.y);

                    const float pad = 6.0f;
                    sf::Vector2f a2 = a;
                    sf::Vector2f b2 = b;
                    if (std::abs(corner.x - a.x) < 0.5f) {
                        a2.y += (corner.y > a.y) ? pad : -pad;
                    } else {
                        a2.x += (corner.x > a.x) ? pad : -pad;
                    }
                    if (std::abs(corner.x - b.x) < 0.5f) {
                        b2.y += (corner.y < b.y) ? -pad : pad;
                    } else {
                        b2.x += (corner.x < b.x) ? -pad : pad;
                    }

                    sf::VertexArray va(sf::LineStrip, 3);
                    va[0] = sf::Vertex(a2, col);
                    va[1] = sf::Vertex(corner, col);
                    va[2] = sf::Vertex(b2, col);
                    window.draw(va);
                };

                drawManhattan(p1, p2, sf::Color::Yellow);
            }
        }

        if (connectNode1_PieceIdx != -1) {
             sf::Vector2f p1 = getAbsoluteNodePos(currentSave.piese[connectNode1_PieceIdx], connectNode1_Index);
             sf::Vector2f pMouse((float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y);
             sf::Vertex line[] = {
                sf::Vertex(p1, sf::Color::Cyan),
                sf::Vertex(pMouse, sf::Color::Cyan)
            };
            window.draw(line, 2, sf::Lines);
        }

        for (int i=0; i<currentSave.piese.size(); i++) {
            sf::Color c = (i == selectedPieceIdx) ? sf::Color::Cyan : sf::Color::Green;
            deseneaza(window, currentSave.piese[i], c);

            sf::Text lbl;
            lbl.setFont(font);
            lbl.setString(currentSave.piese[i].eticheta);
            lbl.setCharacterSize(12);
            lbl.setFillColor(sf::Color::White);
            lbl.setPosition(currentSave.piese[i].x - 10, currentSave.piese[i].y + 20);
            window.draw(lbl);
        }

        sf::Text help;
        help.setFont(font);
        help.setString("Click: Select/Move | Right Click: Rotate | Click Nodes: Connect | E: Edit Label | Del: Delete | S: Save | L: Load");
        help.setCharacterSize(14);
        help.setFillColor(sf::Color(150, 150, 150));
        help.setPosition(10, window.getSize().y - 20);
        window.draw(help);

        window.display();
    }

    return 0;
}
