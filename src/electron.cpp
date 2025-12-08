#include "../include/electron.hpp"
#include "../include/utils.hpp"
#include <cstring>
#include <cmath>
#include <cstdio>
#include <iostream>

void initializeaza(piesa& P) {
    P.orientare = 0;
    strcpy(P.eticheta, P.nume);
    char numeFisier[MAX1 + 5];
    strcpy(numeFisier, P.nume);
    strcat(numeFisier, ".ps");
    
    FILE* f = fopen(numeFisier, "rt");
    if (!f) {
        P.nrLegaturi = 2;
        P.pLeg[1].x = -2; P.pLeg[1].y = 0;
        P.pLeg[2].x = 2;  P.pLeg[2].y = 0;
        P.descr.nComenzi = 1;
        P.descr.comanda[1] = 'R'; 
        P.descr.x1[1] = -1.5; P.descr.y1[1] = -1;
        P.descr.x2[1] = 1.5;  P.descr.y2[1] = 1;
        return;
    }

    char s[MAX1];
    fscanf(f, "%s", s);
    fscanf(f, "%d", &P.nrLegaturi);
    for (unsigned i = 1; i <= P.nrLegaturi; i++) {
        fscanf(f, "%f%f", &P.pLeg[i].x, &P.pLeg[i].y);
    }
    fscanf(f, "%s", s);
    fscanf(f, "%d", &P.descr.nComenzi);
    for (unsigned i = 1; i <= P.descr.nComenzi; i++) {
        fscanf(f, " %c%f%f", &P.descr.comanda[i], &P.descr.x1[i], &P.descr.y1[i]);
        fscanf(f, "%f%f", &P.descr.x2[i], &P.descr.y2[i]);
    }
    fclose(f);
}

void myLine(sf::RenderWindow& window, piesa P, unsigned i, sf::Color color) {
    float x_1, y_1, x_2, y_2;
    float x1 = P.descr.x1[i]; float y1 = P.descr.y1[i];
    float x2 = P.descr.x2[i]; float y2 = P.descr.y2[i];

    float tx1=x1, ty1=y1, tx2=x2, ty2=y2;
    float rx, ry;
    for(unsigned k=0; k<P.orientare; k++) {
        roteste(tx1, ty1, rx, ry); tx1=rx; ty1=ry;
        roteste(tx2, ty2, rx, ry); tx2=rx; ty2=ry;
    }
    x_1 = tx1; y_1 = ty1; x_2 = tx2; y_2 = ty2;

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(P.x + zoom * x_1, P.y + zoom * y_1), color),
        sf::Vertex(sf::Vector2f(P.x + zoom * x_2, P.y + zoom * y_2), color)
    };
    window.draw(line, 2, sf::Lines);
}

void myRectangle(sf::RenderWindow& window, piesa P, unsigned i, sf::Color color) {
    float x1 = P.descr.x1[i]; float y1 = P.descr.y1[i];
    float x2 = P.descr.x2[i]; float y2 = P.descr.y2[i];
    
    float ptsX[] = {x1, x2, x2, x1};
    float ptsY[] = {y1, y1, y2, y2};
    
    sf::VertexArray quad(sf::LineStrip, 5);
    for(int j=0; j<4; j++) {
        float px = ptsX[j], py = ptsY[j];
        float rpx, rpy;
        float tpx = px, tpy = py;
        for(unsigned k=0; k<P.orientare; k++) {
            roteste(tpx, tpy, rpx, rpy); tpx=rpx; tpy=rpy;
        }
        quad[j].position = sf::Vector2f(P.x + tpx*zoom, P.y + tpy*zoom);
        quad[j].color = color;
    }
    quad[4] = quad[0];
    window.draw(quad);
}

void myEllipse(sf::RenderWindow& window, piesa P, unsigned i, sf::Color color) {
    float x1 = P.descr.x1[i]; float y1 = P.descr.y1[i];
    float r1 = P.descr.x2[i]; float r2 = P.descr.y2[i];

    float cx = x1, cy = y1;
    float rcx, rcy;
    for(unsigned k=0; k<P.orientare; k++) {
        roteste(cx, cy, rcx, rcy); cx=rcx; cy=rcy;
        if (k%2==0) { } else { std::swap(r1, r2); } 
    }

    sf::CircleShape circle(r1 * zoom);
    circle.setOrigin(r1 * zoom, r1 * zoom);
    circle.setPosition(P.x + cx * zoom, P.y + cy * zoom);
    if(r1 != 0) circle.setScale(1.0f, r2/r1);
    
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(color);
    circle.setOutlineThickness(1.0f);
    window.draw(circle);
}

void myArc(sf::RenderWindow& window, piesa P, unsigned i, sf::Color color) {
    float x1 = P.descr.x1[i]; float y1 = P.descr.y1[i];
    float r = P.descr.x2[i];
    
    float startAngle = 0, endAngle = 180;
    for(unsigned k=0; k<P.orientare; k++) {
        startAngle += 90;
        endAngle += 90;
        float rx, ry;
        roteste(x1, y1, rx, ry); x1=rx; y1=ry;
    }

    drawSfmlArc(window, P.x + x1*zoom, P.y + y1*zoom, r*zoom, startAngle, endAngle, color);
}

void deseneaza(sf::RenderWindow& window, piesa P, sf::Color color) {
    for (unsigned i = 1; i <= P.descr.nComenzi; i++) {
        switch (P.descr.comanda[i]) {
            case 'L': myLine(window, P, i, color); break;
            case 'R': myRectangle(window, P, i, color); break;
            case 'O': myEllipse(window, P, i, color); break;
            case 'A': myArc(window, P, i, color); break;
        }
    }
    
    for (unsigned i = 1; i <= P.nrLegaturi; i++) {
        sf::Vector2f pos = getAbsoluteNodePos(P, i);
        sf::CircleShape node(3);
        node.setOrigin(1.5, 1.5);
        node.setPosition(pos);
        node.setFillColor(sf::Color::Red);
        window.draw(node);
    }
}

void deseneazaMeniul(sf::RenderWindow& window, piesa Meniu[MAX2], unsigned nrPieseMeniu, sf::Font& font) {
    sf::RectangleShape menuBg(sf::Vector2f(window.getSize().x, 60));
    menuBg.setFillColor(sf::Color(50, 50, 50));
    window.draw(menuBg);

    int lat = window.getSize().x / nrPieseMeniu;

    for (unsigned i = 1; i <= nrPieseMeniu; i++) {
        Meniu[i].x = lat * i - lat / 2;
        Meniu[i].y = 30;
        
        deseneaza(window, Meniu[i], sf::Color::White);

        sf::Text text;
        text.setFont(font);
        text.setString(Meniu[i].nume);
        text.setCharacterSize(10);
        text.setFillColor(sf::Color::White);
        text.setPosition(Meniu[i].x - 15, 45);
        window.draw(text);
    }
}

int findPieceAt(const std::vector<piesa>& piese, int mx, int my) {
    for(int i = piese.size()-1; i >= 0; i--) {
        if (abs(piese[i].x - mx) <= 30 && abs(piese[i].y - my) <= 30) {
            return i;
        }
    }
    return -1;
}

bool findNodeAt(const std::vector<piesa>& piese, int mx, int my, int& outPiesaIdx, int& outNodIdx) {
    for(int i = 0; i < piese.size(); i++) {
        for(unsigned j = 1; j <= piese[i].nrLegaturi; j++) {
            sf::Vector2f pos = getAbsoluteNodePos(piese[i], j);
            if(abs(pos.x - mx) <= 8 && abs(pos.y - my) <= 8) {
                outPiesaIdx = i;
                outNodIdx = j;
                return true;
            }
        }
    }
    return false;
}

int findPieceIndexById(const std::vector<piesa>& piese, int id) {
    for(int i=0; i<piese.size(); i++) if(piese[i].id == id) return i;
    return -1;
}
