#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

#define MAX1 20
#define MAX2 40
#define PI 3.14159265

extern float zoom;
extern int gridSize;

struct descriere {
    char comanda[MAX1];
    unsigned nComenzi;
    float x1[MAX1], y1[MAX1], x2[MAX1], y2[MAX1];
};

struct punctLegatura {
    float x, y; // Coordonate relative la centrul piesei
};

struct piesa {
    int id; // ID unic pentru legaturi
    char nume[MAX1];
    int x, y; // Pozitie pe ecran
    unsigned orientare; // 0, 1, 2, 3
    char eticheta[MAX2]; // Valoare/Nume editabil
    unsigned nrLegaturi;
    punctLegatura pLeg[MAX1];
    descriere descr;
};

struct Legatura {
    int piesa1_id;
    int nod1_index;
    int piesa2_id;
    int nod2_index;
};

struct saveFile {
    vector<piesa> piese;
    vector<Legatura> legaturi;
};