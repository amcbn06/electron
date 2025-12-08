#include "../include/utils.hpp"
#include <cmath>

void roteste(float x, float y, float &xnou, float &ynou) {
    xnou = y;
    ynou = -x;
}

sf::Vector2f getAbsoluteNodePos(const piesa& P, int nodeIndex) {
    float x_rel = P.pLeg[nodeIndex].x;
    float y_rel = P.pLeg[nodeIndex].y;
    float x_rot, y_rot;

    // Aplicam rotatia coordonatelor relative
    float tx = x_rel, ty = y_rel;
    for(unsigned k=0; k<P.orientare; k++) {
        roteste(tx, ty, x_rot, y_rot);
        tx = x_rot; ty = y_rot;
    }
    
    // Aplicam zoom si pozitia piesei
    return sf::Vector2f(P.x + tx * zoom, P.y + ty * zoom);
}

void drawSfmlArc(sf::RenderWindow& window, float x, float y, float radius, float startAngle, float endAngle, sf::Color color) {
    sf::VertexArray lines(sf::LineStrip);
    int segments = 20;
    float step = (endAngle - startAngle) / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = (startAngle + step * i) * (PI / 180.0f);
        float px = x + radius * cos(angle);
        float py = y - radius * sin(angle); 
        lines.append(sf::Vertex(sf::Vector2f(px, py), color));
    }
    window.draw(lines);
}
