#pragma once
#include <SFML/Graphics.hpp>

namespace Constants
{
    inline const float gridStep = 40.0f;

    // Recommended range between 0.03 and 0.1
    inline const float zoomSensitivity = 0.1f;

    // #TODO: idk how to implement panning sensitivity
    inline const float panningSensitivity = 1.0f;

    const float zoomAlpha = 0.5;
    const long double paddingDelay = 0.05; // in seconds

    const float text_size = 20.0f;

    const float select_thr = 30;
    
    const float min_dist_comps = 200;

    const std::map<std::string, std::tuple<std::string, float, float>> componente = {
        {"servo_motor", {"grade", 90.0f, 10}},
        {"baterie", {"V", 12.0f, 20}},
        {"condensator_polarizat", {"uF", 100.0f, 20}},
        {"dioda_zenner", {"V", 5.1f, 20}},
        {"tranzistor_npn", {"A", 0.5f, 20}},
        {"nand", {"V", 5.0f, 20}},
        {"condensator", {"nF", 10.0f, 20}},
        {"tranzistor_pnp", {"A", 0.5f, 20}},
        {"rezistenta", {"Ohm", 1000.0f, 20}},
        {"dioda", {"V", 0.7f, 20}},
        {"amplificator", {"gain", 100.0f, 15}}};
}