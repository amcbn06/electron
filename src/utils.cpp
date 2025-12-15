#include "../include/utils.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstring>

void roteste(float x, float y, float &xnou, float &ynou)
{
    xnou = y;
    ynou = -x;
}

sf::Vector2f getAbsoluteNodePos(const piesa &P, int nodeIndex)
{
    float x_rel = P.pLeg[nodeIndex].x;
    float y_rel = P.pLeg[nodeIndex].y;
    float x_rot, y_rot;

    // Aplicam rotatia coordonatelor relative
    float tx = x_rel, ty = y_rel;
    for (unsigned k = 0; k < P.orientare; k++)
    {
        roteste(tx, ty, x_rot, y_rot);
        tx = x_rot;
        ty = y_rot;
    }

    // Aplicam zoom si pozitia piesei
    return sf::Vector2f(P.x + tx * zoom, P.y + ty * zoom);
}

void drawSfmlArc(sf::RenderWindow &window, float x, float y, float radius, float startAngle, float endAngle, sf::Color color)
{
    sf::VertexArray lines(sf::LineStrip);
    int segments = 20;
    float step = (endAngle - startAngle) / segments;

    for (int i = 0; i <= segments; ++i)
    {
        float angle = (startAngle + step * i) * (PI / 180.0f);
        float px = x + radius * cos(angle);
        float py = y - radius * sin(angle);
        lines.append(sf::Vertex(sf::Vector2f(px, py), color));
    }
    window.draw(lines);
}

void save(int id, saveFile currentSave)
{
    std::filesystem::create_directories("saves");
    std::string path = "saves/" + std::to_string(id) + ".txt";
    std::ofstream ofs(path);
    if (!ofs) { std::cerr << "Failed to open " << path << "\n"; return; }

    ofs << "Salvare circuit\n";

    ofs << currentSave.piese.size() << "\n";
    for (const auto &p : currentSave.piese) {
        ofs << p.id << " " << std::string(p.nume) << " " << p.x << " " << p.y << " " << p.nrLegaturi << "\n";
        for (unsigned i = 1; i <= p.nrLegaturi; ++i)
            ofs << p.pLeg[i].x << " " << p.pLeg[i].y << "\n";
    }

    ofs << currentSave.legaturi.size() << "\n";
    for (const auto &l : currentSave.legaturi) {
        ofs << l.piesa1_id << " " << l.nod1_index << " " << l.piesa2_id << " " << l.nod2_index << "\n";
    }
}

saveFile load(int id)
{
    saveFile currentSave;
    std::string path = "saves/" + std::to_string(id) + ".txt";
    std::ifstream ifs(path);
    if (!ifs) { std::cerr << "Failed to open " << path << "\n"; return currentSave; }

    std::string header;
    if (!std::getline(ifs, header) || header != "Salvare circuit") {
        std::cerr << "Unsupported or corrupt save file: " << path << "\n";
        return currentSave;
    }

    size_t nP = 0;
    ifs >> nP;
    currentSave.piese.clear();
    for (size_t pi = 0; pi < nP; ++pi) {
        piesa p{};
        int pid = 0;
        std::string name;
        ifs >> pid >> name >> p.x >> p.y >> p.nrLegaturi;
        p.id = pid;
        std::strncpy(p.nume, name.c_str(), MAX1 - 1);
        for (unsigned i = 1; i <= p.nrLegaturi; ++i) {
            float fx = 0, fy = 0;
            ifs >> fx >> fy;
            p.pLeg[i].x = fx; p.pLeg[i].y = fy;
        }
        p.descr.nComenzi = 0;
        currentSave.piese.push_back(p);
    }

    size_t nL = 0;
    ifs >> nL;
    currentSave.legaturi.clear();
    for (size_t li = 0; li < nL; ++li) {
        Legatura l;
        ifs >> l.piesa1_id >> l.nod1_index >> l.piesa2_id >> l.nod2_index;
        currentSave.legaturi.push_back(l);
    }

    return currentSave;
}