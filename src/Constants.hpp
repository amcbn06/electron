#pragma once
#include <SFML/Graphics.hpp>

namespace Constants {
    inline const float gridStep = 40.0f;

    // Recommended range between 0.03 and 0.1
    inline const float zoomSensitivity = 0.1f;
    
    // #TODO: idk how to implement panning sensitivity
    inline const float panningSensitivity = 1.0f;

    const float zoomAlpha = 0.5;
    const long double paddingDelay = 0.07; // in seconds
}